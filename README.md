## Notice  
You may be interested in my other project, which is a simple PInvoke class for WinDivert. It does require unsafe code, but it's much less hassle than using this library. You can find it [here on github](https://github.com/TechnikEmpire/DivertPInvoke).

# Divert.Net

.NET Wrapper for [WinDivert](https://github.com/basil00/Divert), available under the very permissive MIT License. It should be noted that the library that this project wraps, WinDivert, is [licensed differently](https://github.com/basil00/Divert/blob/master/LICENSE). Don't forget to respect the license of WinDivert.

## Goals

 - Wrap WinDivert in a managed, easy to use CLR library.
 - Provide helpful extensions for commonly desired tasks, such as the ability to identify the process behind a packet flow.
 - Win?

The tests from WinDivert have been ported and all tests are passing. First release coming soon with an upload to nuget.

Before trying to build, there's a very small amount of configuration required, all of which is explained on the [Wiki page](https://github.com/TechnikEmpire/Divert.Net/wiki).
