[Table of contents](_tableOfContents.md)  
Previous: [Information for developers](developers.md) | Next: [Detailed description of pseudonymization algorithm](techDetails.md)

# _Fsseudonymizer:_ Customizing the application

_Fsseudonyomizer_ is designed to be useful "out of the box" for most purposes, but also easily modifiable for different uses that might require more sophisticated data processing.

Some options for customizing the program, such as modifying the Rules file, require no programming capability.  More complex changes will require the writing and integration of new computer code in C++: the provided [information for developers](developers.md) may be useful.

## Modifying the Rules file

*Note that some customized editions of _Fsseudonyomizer_ may not have a user-editable Rules file: in these cases, this file is built in to the application, and cannot be directly modified without rebuilding the application.*

The simplest customization requires only modification of the [Rules file](rules.md): the way in which fields are processed can be changed simply by modifying entries in the Rules file.  Options available in the Rules file are described in detail [here](rules.md).

## Building the Rules file into the program

It may be simpler in some instances for developers to build Rules file capabilities directly into the _Fsseudonyomizer_ application.  This can be done by incorporating a Rules file (in .csv, .xls, or .xlsx format) as a [resource](https://doc.qt.io/qt-5/resources.html) into the application.  When _Fsseudonyomizer_ is run, it will automatically detect the presence of a built-in Rules file, and will not prompt the data provider running the program to specify one of her own.

## More advanced customizations

More complex modifications to data processing may be incorporated by modifying the functions in the file ```customprocessing.cpp```.  Modifications to the entire data set (such as adding, removing, or changing entire fields) or to individual records (for example, incorporating more sophisticated data validation requirements, or splitting a field in a record into two or more fields, or combining fields in a record to a single value) can be made.

These changes may be made before the rules in a Rules file are applied (preprocessing) or after pseudonymization rules have been applied (postprocessing).



