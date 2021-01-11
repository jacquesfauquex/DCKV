# DICOM ordered contextualizedKey-values JSON

Although it has always been possible to transform any of the three official DICOM representations  (binary, native xml, json) into any other of the three, the organizational principles of each of them were highly language dependant and required organizational traductions".

Digging deeper in the commonalities between the three idioms, especially considering the bridge tended between __[json](http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf)__ and xml in __[xslt 3.1](https://www.w3.org/TR/xslt-30/#json)__, we found corresponding building blocks in xml and json. It goes one step further than a mere traduction. 

*It is possible to read json directly from xslt edit it using the building blocks in the xslt 3.1 language and then write the result in json:*
- __[func-json-to-xml](https://www.w3.org/TR/xslt-30/#func-json-to-xml)__
- xslt3.1 includes many specific functions operating on maps __[merge, size, keys, contains, get, put, entry, remove, for-each, find, ...](https://www.w3.org/TR/xslt-30/#func-xml-to-json)__
- __[func-xml-to-json](https://www.w3.org/TR/xslt-30/#func-xml-to-json)__

Json tools are far less powerfull than xml's for edition. But on the other hand, since json is the optimal choice for javascript,it is now posible to beneficiate the best of both words without modification of the building blocks.
