# SeshNx Chronos

**Professional Delay Plugin with Tempo-Sync, Modulation, and Creative Effects**

Chronos is a feature-rich delay plugin designed for modern music production. With tempo-synced delays, LFO modulation, feedback filtering, and creative features like ducking and freeze, Chronos provides everything from subtle echoes to experimental soundscapes.

## Features

### Core Delay
- **Delay Time**: 1ms to 2000ms continuous range
- **Tempo Sync**: 1/64 to 2 bars with triplet and dotted options
- **High-Quality Interpolation**: Cubic interpolation for smooth modulation

### Feedback Processing
- **Feedback**: 0-100% with soft saturation to prevent runaway
- **Filter in Loop**: Low Pass, High Pass, or Band Pass with resonance
- **Damping**: High-frequency rolloff per repeat (tape-style decay)
- **Drive**: Subtle saturation in feedback path

### Modulation
- **LFO Rate**: 0.01 - 20 Hz (or tempo-synced)
- **Depth**: 0-100% delay time modulation
- **Shapes**: Sine, Triangle, Random (Sample & Hold)

### Stereo Modes
- **Mono**: Both channels receive identical delay
- **Stereo**: Independent L/R with optional offset
- **Ping-Pong**: Classic alternating stereo bounce
- **Wide**: M/S processing for enhanced stereo width

### Creative Features
- **Ducking**: Delay automatically ducks when input is present
- **Freeze**: Infinite hold of current delay buffer

### I/O
- **Input/Output Gain**: -24dB to +12dB
- **Mix**: 0-100% wet/dry blend
- **Width**: Stereo spread control

## System Requirements

- **Windows**: Windows 10 or later (64-bit)
- **macOS**: macOS 10.13 or later
- **Linux**: Ubuntu 20.04 or compatible

### Supported Formats
- VST3
- Audio Unit (AU) - macOS only
- Standalone
- AAX (requires AAX SDK)

## Building from Source

### Prerequisites
- CMake 3.22 or higher
- C++17 compatible compiler
- For AAX: Avid AAX SDK (set path in CMakeLists.txt)

### Build Instructions

```bash
# Clone repository
git clone https://github.com/your-repo/Chronos.git
cd Chronos

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release
```

### Build Output Locations
- **VST3**: `build/Chronos_artefacts/Release/VST3/`
- **AU**: `build/Chronos_artefacts/Release/AU/`
- **Standalone**: `build/Chronos_artefacts/Release/Standalone/`
- **AAX**: `build/Chronos_artefacts/Release/AAX/`

## Parameter Reference

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Delay Time | 1-2000 ms | 250 ms | Main delay time |
| Tempo Sync | On/Off | Off | Sync delay to host tempo |
| Sync Division | 1/64 - 2 bars | 1/4 | Beat division when synced |
| Feedback | 0-100% | 30% | Amount of signal fed back |
| Filter Freq | 20-20000 Hz | 8000 Hz | Feedback filter cutoff |
| Filter Res | 0-100% | 0% | Feedback filter resonance |
| Filter Mode | LP/HP/BP | LP | Feedback filter type |
| Damping | 0-100% | 30% | High-frequency rolloff per repeat |
| Drive | 0-100% | 0% | Saturation in feedback |
| Mod Rate | 0.01-20 Hz | 0.5 Hz | LFO modulation rate |
| Mod Depth | 0-100% | 0% | LFO modulation depth |
| Mod Shape | Sine/Tri/Rand | Sine | LFO waveform |
| Stereo Mode | Mono/Stereo/PP/Wide | Stereo | Stereo processing mode |
| Width | 0-200% | 100% | Stereo width |
| Ducking | On/Off | Off | Enable ducking |
| Duck Amount | 0-100% | 50% | Ducking intensity |
| Freeze | On/Off | Off | Freeze delay buffer |
| Input Gain | -24 to +12 dB | 0 dB | Input level |
| Output Gain | -24 to +12 dB | 0 dB | Output level |
| Mix | 0-100% | 50% | Wet/dry blend |

## License

Copyright (c) 2024 Amalia Media LLC / SeshNx. All rights reserved.

## Support

For bug reports and feature requests, please open an issue on GitHub.
