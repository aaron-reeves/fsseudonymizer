[Table of contents](_tableOfContents.md)  
Previous: [Customizing the application](customizing.md) | Next: [Software license](license.md)

# _Fsseudonymizer:_ Detailed description of pseudonymization algorithm

In our work, we occasionally encounter situations in which we might wish to use data that identifies individual people, businesses, animals, or facilities.  It is often essential, for example, that we are able to follow events that occur on an individual premises, or to pull together information from several different sources that pertain to a particular person, animal, or location.

In many of these cases, however, such data is sensitive or confidential: information provided by businesses, for example, might be confidential, and personal information pertaining to individuals is often protected by law.

In order for our work to be successful, then, two seemingly contradictory criteria must be fulfilled:

  - Personal or confidential data that might reveal the specific identity of a person, or a precise location of a particular premises cannot be revealed.
  - However, in order to ensure the validity of our work, it must be possible to uniquely identify all individuals, premises, etc., across multiple data sets.

In order to address both of these requirements, we have developed an approach for processing data, described below, that allows the assignment of unique pseudonymous identifiers to individuals or entities without revealing confidential or personal information.  These approaches can be applied to data from multiple sources by the data providers themselves, thus never revealing any personal or confidential information to any unauthorized individual or organization.

This approach makes use of a cryptographic hash and a secure pass phrase, together with a custom computer application, for generating pseudonymized versions of data that will be suitable for subsequent needs.  Each of these is described in the following sections.

## The cryptographic hash

Cryptographic hash algorithms are used to encrypt digital data.  Cryptographic hash algorithms are widely used for website security, for password verification, to verify data integrity, and to identify individual pieces of data without revealing their content.  When such algorithm is applied to a value or a file, a new, unique representation of that data, called a hash value, is generated.  (Although it is theoretically possible that two different pieces of data might generate the same hash value, the probability that such an event will occur is exceedingly remote.  Given current computational capabilities, estimates of the amount of time required to find two pieces of data that produce the same hash value range from thousands to trillions of years: see, for example, <http://ericsink.com/vcbe/html/cryptographic_hashes.html>.)  This process is irreversible: it is virtually impossible, given only a hash value, to regenerate the original input data.

_Fsseudonymizer_ employs a hash algorithm from a family of related algorithms referred to as SHA-2.  These algorithms, developed by the United States government, are currently recommended by the US government and many other entities for the protection of sensitive data.  They are the same standard employed in the TSL and SSL protocols, which are commonly used for internet-based commerce and electronic banking. Table 1 illustrates the application of a SHA-2 cryptographic hash algorithm to several simple pieces of data.

_**Table 1.**  Sample data, before and after processing with a SHA-2 cryptographic hash algorithm.  Note the dissimilarity among hash values, even between similar original pieces of data._

| Original data | Hash value                                                     |
|---------------|----------------------------------------------------------------|
| red           | ```05bb61945cb39c4e40d68db996efb2501f10e699b45b78096b7e3978``` |
| apple         | ```b7bbfdf1a1012999b3c466fdeb906a629caa5e3e022428d1eb702281``` |
| 1234          | ```99fb2f48c6af4761f904fc85f95eb56190e5d40b1f44ec3a9c1fa319``` |
| 1243          | ```4fe594cf22c7f6976fa710cba7bb2f74659b96bc38a5ce2ee65454b1``` |

## The pass phrase

Although original data cannot be recovered from hash values, it would be possible for relatively simple, predictable data formats to deduce what the original data was.  Consider, for example, a commonly used farm identifier, the CPH number.  CPH numbers all have a consistent, predictable format, and for CPH numbers used in Great Britain, there are only one billion possible values.  It would be possible to generate a list of hash values for all possible CPH numbers.  By comparing a particular hash value to those on this list, one could determine what the original data must have been.


Although it is not possible to recover original data from its hash value, it would be possible for relatively simple, predictable data formats to deduce what the original data was.  For example, consider American Social Security Numbers (SSN).  Every SSN is a 9-digit number.  There are only one billion possible social security numbers.  It would be possible to generate a list of hash values for all one billion possibilities.  If it were known that a piece of original data was a Social Security Number, one could determine what the original data must have been by comparing a particular hash value to all of those those on this list.

By itself, then, employment of a cryptographic hash function is insufficient to ensure that confidentiality is protected.  By incorporating an additional level of security, however, we can ensure that it is virtually impossible to perform this kind of so-called brute-force approach to effectively break the encryption.  For this, we propose the use of a pass phrase.

A pass phrase is a string of several generally randomly selected words that would be difficult to guess.  When a pass phrase is combined with a piece of data and then both subjected to a cryptographic hash algorithm, a unique data identifier is still generated, but without access to the pass phrase, it becomes infeasible to apply brute-force techniques to deduce the contents of the original data.

For use with _Fsseudonymizer_, we suggest the use of a pass phrase of four to six words, to be generated by data providers and held confidentially, with the explicit requirement that the pass phrase is not to be given to data users.

Table 2 illustrates the application of a SHA-2 cryptographic hash algorithm to several simple pieces of data that have been combined with a four-word pass phrase.

_**Table 2.**  Sample data, before and after processing with a SHA-2 cryptographic hash algorithm.  In each case, the pass phrase “duckbill bent limbate hamlet” was prepended to the original data prior to encryption.  Note the dissimilarity between hash values here and those shown in Table 1, which did not use a pass phrase._

| Original data | Hash value                                                     |
|---------------|----------------------------------------------------------------|
| red           | ```39d0fb1f49064ea616c763505c91c15ddabd092e538eb3fc1544043a``` |
| apple         | ```92f054b2b65ba12f80fc929146106896cfd2871da9ed4d2aeb7adbc6``` |
| 1234          | ```58c35c9915e67f77f38d1a274bdba706dc06df6f7451e1524979e26f``` |
| 1243          | ```37caa8ae844a38761b63f3aa7d76b4478a02262dc143dfeeedb2bada``` |

## Useful resources

The following websites, and references contained therein, provide additional background information for the topics discussed here.

  - [Cryptographic hash functions](http://en.wikipedia.org/wiki/Cryptographic_hash_function)
  - [The SHA-2 family of cryptographic hash functions](http://en.wikipedia.org/wiki/SHA-2)
  - [A description of the use of cryptographic hash values for data identification](http://ericsink.com/vcbe/html/cryptographic_hashes.html)
  - [A description of pass phrases and their application](http://en.wikipedia.org/wiki/Passphrase)
  - [Guidelines for generating strong pass phrases](http://windows.microsoft.com/en-gb/windows7/tips-for-creating-strong-passwords-and-passphrases)
  - [An application for randomly generating a pass phrase](http://www.fourmilab.ch/javascrypt/pass_phrase.html)
