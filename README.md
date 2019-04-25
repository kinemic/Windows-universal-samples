<!---
  samplefwlink: http://go.microsoft.com/fwlink/p/?LinkId=619979
--->

# Kinemic Gesture SDK - Universal Windows Platform (UWP) app samples

This repo contains the samples that demonstrate the API usage patterns for the Kinemic Software Development Kit (SDK) on the Universal Windows Platform (UWP) for Windows 10. 
These code samples were adapted from [Microsoft/Windows-universal-samples](https://github.com/Microsoft/Windows-universal-samples) and are designed to run on desktop, mobile, and future devices that support the Universal Windows Platform.

> **Note:** If you are unfamiliar with Git and GitHub, you can download the entire collection as a 
> [ZIP file](../../archive/master.zip), but be 
> sure to unzip everything to access shared dependencies.
> For more samples, see our [GitHub Samples](https://github.com/Kinemic) or our [Developer Area](https://developer.kinemic.com). 

## SDK Access

These code samples require the Kinemic Gesture SDK which is available through NuGet, after you purchased a Kinemic Band.
For visual studio to automatically install the Kinemic Gesture SDK NuGet package, you have to add our NuGet repository to your NuGet sources, this can be done for the NuGet CLI or in Visual Studio

### NuGet CLI Configuration

To configure the NuGet CLI to work with Artifactory, you need to add this repository to the list of sources.
To add this repository, use the following command:

```nuget sources Add -Name Kinemic_Develop -Source https://kinemic.jfrog.io/kinemic/api/nuget/nuget-develop -username <USERNAME> -password <PASSWORD>```

Then, to authenticate against Artifactory with the NuGet API key, run the following command:

```nuget setapikey <USERNAME>:<PASSWORD> -Source Kinemic_Develop```

### Visual Studio Configuration

To configure the NuGet Visual Studio Extension to use Artifactory, you need to add this repository as another Package Source under NuGet Package Manager.

1. Go to the "Package Manager Settings" in your Visual Studio (Tools > NuGet Package Manager > Package Manager Settings > Package Sources) and add another Package Source.
2. Name: Add a name for the package source (e.g. Artifactory NuGet repository)
3. Paste the snippet below in the URL field

```https://kinemic.jfrog.io/kinemic/api/nuget/nuget-develop```

## Universal Windows Platform development

These samples require Visual Studio 2017 Update 4 or higher and the Windows Software Development Kit (SDK) version 17134 for Windows 10.

   [Get a free copy of Visual Studio 2017 Community Edition with support for building Universal Windows Platform apps](http://go.microsoft.com/fwlink/p/?LinkID=280676)

## Using the samples

The easiest way to use these samples without using Git is to download the zip file containing the current version (using the following link or by clicking the "Download ZIP" button on the repo page). You can then unzip the entire archive and use the samples in Visual Studio 2017.

   [Download the samples ZIP](../../archive/master.zip)

   **Notes:** 
   * Before you unzip the archive, right-click it, select **Properties**, and then select **Unblock**.
   * Be sure to unzip the entire archive, and not just individual samples. The samples all depend on the SharedContent folder in the archive.   
   * In Visual Studio 2017, the platform target defaults to ARM, so be sure to change that to x64 or x86 if you want to test on a non-ARM device. 
   
The samples use Linked files in Visual Studio to reduce duplication of common files, including sample template files and image assets. These common files are stored in the SharedContent folder at the root of the repository, and are referred to in the project files using links.

**Reminder:** If you unzip individual samples, they will not build due to references to other portions of the ZIP file that were not unzipped. You must unzip the entire archive if you intend to build the samples.

For more info about the programming models, platforms, languages, and APIs demonstrated in these samples, please refer to the guidance, tutorials, and reference topics provided in the Kinemic Gesture SDK documentation available in the [Kinemic Developer Area](https://developer.kinemic.com). These samples are provided as-is in order to indicate or demonstrate the functionality of the programming models and feature APIs for Windows.

## See also

For more information about the Kinemic Gesture SDK, visit our [Developer Area](https://developer.kinemic.com).

## Adapted Samples

<table>
  <tr>
   <td><a href="Samples/PdfDocument">PDF document</a></td>
  </tr>
</table>