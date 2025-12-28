#include "PluginProcessor.h"
#include "PluginEditor.h"

ChronosAudioProcessor::ChronosAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", Chronos::Parameters::createLayout()),
      params(apvts)
{
}

ChronosAudioProcessor::~ChronosAudioProcessor()
{
}

const juce::String ChronosAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChronosAudioProcessor::acceptsMidi() const
{
    return false;
}

bool ChronosAudioProcessor::producesMidi() const
{
    return false;
}

bool ChronosAudioProcessor::isMidiEffect() const
{
    return false;
}

double ChronosAudioProcessor::getTailLengthSeconds() const
{
    return 2.0;  // Max delay time
}

int ChronosAudioProcessor::getNumPrograms()
{
    return 1;
}

int ChronosAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChronosAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String ChronosAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void ChronosAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void ChronosAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    delayEngine.prepare(static_cast<float>(sampleRate), samplesPerBlock);
}

void ChronosAudioProcessor::releaseResources()
{
    delayEngine.reset();
}

bool ChronosAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void ChronosAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    // Get tempo from host
    if (auto* playHead = getPlayHead())
    {
        if (auto posInfo = playHead->getPosition())
        {
            if (posInfo->getBpm().hasValue())
                currentBPM = static_cast<float>(*posInfo->getBpm());
        }
    }

    // Build engine parameters from APVTS
    Chronos::DelayEngine::Parameters engineParams;

    // Time
    bool tempoSyncEnabled = params.tempoSync->load() > 0.5f;
    bool linkLR = params.linkLR->load() > 0.5f;

    if (tempoSyncEnabled)
    {
        auto division = static_cast<Chronos::SyncDivision>(static_cast<int>(params.syncDivision->load()));
        engineParams.delayTimeMs = Chronos::calculateDelayMs(currentBPM, division);
        engineParams.delayTimeRightMs = engineParams.delayTimeMs;
    }
    else
    {
        engineParams.delayTimeMs = params.delayTime->load();
        engineParams.delayTimeRightMs = linkLR ? engineParams.delayTimeMs : params.delayTimeR->load();
    }

    // Feedback
    engineParams.feedback = params.feedback->load() / 100.0f;
    engineParams.filterFreq = params.fbFilterFreq->load();
    engineParams.filterRes = params.fbFilterRes->load() / 100.0f;
    engineParams.filterMode = static_cast<Chronos::FilterMode>(static_cast<int>(params.fbFilterMode->load()));
    engineParams.damping = params.damping->load() / 100.0f;
    engineParams.drive = params.drive->load() / 100.0f;

    // Modulation
    bool modSyncEnabled = params.modSync->load() > 0.5f;
    if (modSyncEnabled)
    {
        auto modDiv = static_cast<Chronos::SyncDivision>(static_cast<int>(params.modSyncDiv->load()));
        engineParams.modRateHz = Chronos::calculateLFORateHz(currentBPM, modDiv);
    }
    else
    {
        engineParams.modRateHz = params.modRate->load();
    }
    engineParams.modDepth = params.modDepth->load() / 100.0f;

    // Set LFO shape
    delayEngine.setLFOShape(static_cast<Chronos::LFOShape>(static_cast<int>(params.modShape->load())));

    // Stereo
    engineParams.stereoMode = static_cast<Chronos::StereoMode>(static_cast<int>(params.stereoMode->load()));
    engineParams.width = params.width->load() / 100.0f;

    // Features
    engineParams.duckingEnabled = params.ducking->load() > 0.5f;
    engineParams.duckAmount = params.duckAmount->load() / 100.0f;
    engineParams.freeze = params.freeze->load() > 0.5f;

    // I/O (convert dB to linear)
    engineParams.inputGain = juce::Decibels::decibelsToGain(params.inputGain->load());
    engineParams.outputGain = juce::Decibels::decibelsToGain(params.outputGain->load());
    engineParams.mix = params.mix->load() / 100.0f;

    delayEngine.setParameters(engineParams);

    // Process audio
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    delayEngine.process(leftChannel, rightChannel, buffer.getNumSamples());
}

bool ChronosAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ChronosAudioProcessor::createEditor()
{
    return new ChronosAudioProcessorEditor(*this);
}

void ChronosAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ChronosAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChronosAudioProcessor();
}
