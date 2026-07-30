[![aravis-yarp-devices Homepage](https://img.shields.io/badge/roboticslab-aravis_yarp_devices-orange.svg)](https://robots.uc3m.es/aravis-yarp-devices/)

A place for YARP devices related to Aravis.

Link to Doxygen generated documentation: https://robots.uc3m.es/aravis-yarp-devices/

## Credits

Around 2017, David Estévez Fernández built the first version of the `AravisCamera` device (then called `AravisGiGe` due to the GigE Vision camera he was using) and the `grabbercontrols2gui` app during his work on the [Horus](https://github.com/roboticslab-uc3m/horus) project (private repo).

In 2025, Álvaro Santos García generalized the device+GUI pair for USB3 Vision cameras (such as the Point Grey Flea3 FL3-U3-88S2C-C) and built the arm-target-follower application for his Bachelor's Thesis. Full title: *Implementación del software de adquisición de una cámara industrial GenICam para aplicaciones robóticas*.

## Installation

Installation instructions for installing from source can be found [here](doc/aravis-yarp-devices-install.md).

Refer to the usage guides for each component linked in the previous page.

## Contributing

### Posting Issues

1. Read [CONTRIBUTING.md](CONTRIBUTING.md)
2. [Post an issue / Feature request / Specific documentation request](https://github.com/roboticslab-uc3m/aravis-yarp-devices/issues)

### Fork & Pull Request

1. [Fork the repository](https://github.com/roboticslab-uc3m/aravis-yarp-devices/fork)
2. Create your feature branch (`git checkout -b my-new-feature`) off the `master` branch, following the [Forking Git workflow](https://www.atlassian.com/git/tutorials/comparing-workflows/forking-workflow)
3. Commit your changes
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

## Status

[![Continuous Integration](https://github.com/roboticslab-uc3m/aravis-yarp-devices/actions/workflows/ci.yml/badge.svg)](https://github.com/roboticslab-uc3m/aravis-yarp-devices/actions/workflows/ci.yml)

[![Issues](https://img.shields.io/github/issues/roboticslab-uc3m/aravis-yarp-devices.svg?label=Issues)](https://github.com/roboticslab-uc3m/aravis-yarp-devices/issues)

## Citation

Álvaro Santos-García, Bartek Łukawski, Juan G. Victores, Carlos Balaguer, and Alberto Jardón. YARP and ROS 2 GenICam middleware for visual servoing applications. In *XLVII Jornadas de Automática*. Universidade da Coruña, 2026. DOI: [10.17979/ja-cea.2026.47.13836](https://doi.org/10.17979/ja-cea.2026.47.13836)

```bibtex
@inproceedings{santosgarcia2026jjaa,
    author    = {Santos-García, Álvaro and {\L}ukawski, Bartek and Victores, Juan G. and Balaguer, Carlos and Jardón, Alberto},
    title     = {YARP and {ROS 2} GenICam middleware for visual servoing applications},
    booktitle = {XLVII Jornadas de Automática},
    year      = {2026},
    publisher = {Universidade da Coruña},
    doi       = {10.17979/ja-cea.2026.47.13836},
}
```

## See Also

- [roboticslab-uc3m/yarp-devices](https://github.com/roboticslab-uc3m/yarp-devices)
  - [issue #125](https://github.com/roboticslab-uc3m/yarp-devices/issues/125)
  - [issue #145](https://github.com/roboticslab-uc3m/yarp-devices/issues/145)
