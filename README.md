# Clarius Solum API

The Solum API allows third party applications to be developed for desktop or mobile systems that wish to connect to Clarius scanners for imaging functionality. Applications can be executed without the Clarius App running, unlike the Cast API and Mobile API tools. The API only provides connectivity and imaging control; there is no functionality for Clarius Cloud, DICOM, measurements, and other user interface technologies that are integrated into the Clarius App.

## Prerequisites

- [Clarius Ultrasound Scanner](https://clarius.com/)
- Must be a **commercial partner** purchasing bulk units on a quarterly basis
- A valid "oem" license (managed per probe)

## Supported Platforms

- Windows
- Linux
- macOS
- iOS
- Android

### Example Overview

Two desktop example programs are provided, one written as a console program, the other using Qt and a graphical interface. The console program does not use any Bluetooth connectivity, therefore it is only useful if the IP address and port are already known, or if a Bluetooth script is run through the console (for example, using hcitool or gatttool on Linux).

The iOS example program is a simple SwiftUI program that demonstrates some of the features of the framework. To build, the full iOS framework zip must be extracted to the ../../Library/Frameworks/ path or the path must be adjusted in the project settings. A signing certificate must be specified in the project settings. Ensure that the build target is iOS 64-bit arm to match the downloaded framework. The program demonstrates download certificates from Clarius cloud, populating scanner details via bluetooth, and image streaming.

### Documentation

- [Specifications](specifications.md)
- [Design](design.md)

To create a PDF from one of our markdown documents, try the pandoc program: `pandoc document.md -o document.pdf`.
In cases where mermaid is used for diagramming, also pass in: `-F mermaid-filter`, which can be installed via an npm package

## Probe Certificate

A valid probe certificate obtained from Clarius must be set before being able to connect to the probe.

How to obtain a probe certificate:

1. Create an OEM API key from the Clarius Cloud institution admin page: in institution settings > policies > OEM API keys > create a new key and write it down, it will not be shown again. This step needs to be done only once.
2. Download probes certificates at https://cloud.clarius.com/api/public/v0/devices/oem/, use the OEM API key as an authorization token:

```
curl -H "Authorization: OEM-API-Key <your key>" "https://cloud.clarius.com/api/public/v0/devices/oem/"
```

## Probe Firmware

The probe firmware is not embedded in the libraries and must be downloaded from Clarius Cloud.

1. Obtain your OEM API key for Clarius Cloud, it will be used to authorize the HTTP request.
2. Obtain the firmware version with `solumFwVersion()`.
3. Download the firmware at:

```
https://cloud.clarius.com/api/public/v0/devices/oem/firmware/<firmware version>/
```

For example:

```
curl -H "Authorization: OEM-API-Key <your key>" "https://cloud.clarius.com/api/public/v0/devices/oem/firmware/11.3.0-587+670db68186"
```

4. Upload the firmware to the probe with `solumSoftwareUpdate()`.

