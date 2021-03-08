# DiskJunkey
Tweaking around with the Microsoft sysvad driver and interacting with it from a userspace app.

Initial driver sample code was taken from the Microsoft github repository. 
https://github.com/microsoft/Windows-driver-samples/tree/master/audio/simpleaudiosample
The license for the repository under that project was: The Microsoft Public License (MS-PL) and is copied in the dedicated resource folder.
That part of the code is based on commit: 9a2e9aecb9135e782c434e617f57bfbdf9d8f2fd (Add SimpleAudioSample driver. (#577))

The changes following the initial commit will attempt to update the code to give it another purpose: rename devices, and add the necesary ioctl's to feed and access new data from an userspace application.