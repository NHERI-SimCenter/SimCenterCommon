# SimCenterCommon

[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)]( https://nheri-simcenter.github.io/SimCenterCommon/index.html)

|Build OS|Windows|MacOS|Linux|
|---|---|---|---|
|Status|[![Build Status](https://el7addad.visualstudio.com/NHERI%20SimCenter/_apis/build/status/el7addad.SimCenterCommon?branchName=master&jobName=Job&configuration=Job%20windows)](https://el7addad.visualstudio.com/NHERI%20SimCenter/_build/latest?definitionId=2&branchName=master)|[![Build Status](https://el7addad.visualstudio.com/NHERI%20SimCenter/_apis/build/status/el7addad.SimCenterCommon?branchName=master&jobName=Job&configuration=Job%20macHighSierra)](https://el7addad.visualstudio.com/NHERI%20SimCenter/_build/latest?definitionId=2&branchName=master)|[![Build Status](https://el7addad.visualstudio.com/NHERI%20SimCenter/_apis/build/status/el7addad.SimCenterCommon?branchName=master&jobName=Job&configuration=Job%20linux)](https://el7addad.visualstudio.com/NHERI%20SimCenter/_build/latest?definitionId=2&branchName=master)|

## Developer's Guide

This repository is a common library of Qt classes used by the SimCenter Desktop applications.
The repository is organized into separate subfolders, each has a QMake project include file.
All the codes are built into a single static library using QMake and are packaged using Conan package manager.

### Using SimCenterCommon as a dependency

Consuming SimCenterCommon as a dependency can be done using a Conan recipe. The library can be added as a requirement in a ```conanfile.txt``` or ```conanfile.py```. An example ```conanfile.txt``` for consume SimCenterCommon is:

```
[requires]
SimCenterCommonQt/0.1.0@simcenter/testing

[generators]
qmake
```

### Package options

Conan package for SimCenterCommon supports the following options:

|Option Name|Values|Default|Description|
|---|---|---|---|
|MDOFwithQt3D|```True``` or ```False``` |```False```| Rendering MDOF widget using Qt3D|
|withQt|```True``` or ```False```|```False```|Use Qt as a dependency. This allows building the package without having pre-installed Qt (e.g. suitable for automated builds)|
---

### Building and packaging locally
When working on local changes it is possible to package the library locally to use it and test it with a dependent repo. The following command can be used to create the Conan package from local source code:
```
Conan create . simcenter/testing
```
