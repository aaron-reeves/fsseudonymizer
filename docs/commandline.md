[Table of contents](_tableOfContents.md)  
Previous: [The user interface](userInterface.md) | Next: [Information for developers](developers.md)

# _Fsseudonymizer:_ Using the application from the command line

_Fsseudonymizer_ can also be run from the command line, which might be useful if one were to incorporate it as part of a batch script or other automated process.

The commandline version of _Fsseudonymizer_ is named ```fsseudonymizerc.exe```, and is installed as part of the [normal installation process](systemRequirements.md#markdown-header-installation).  It may be helpful to add the _Fsseudonymizer_ installation directory to your [user or system path](https://www.computerhope.com/issues/ch000549.htm), to make access to ```fsseudonymizerc.exe``` more convenient.

Command line arguments are used to specify the input file, user information, and pass phrase.  All command line options are described by the help command:

```
fsseudonymizerc.exe --help
```