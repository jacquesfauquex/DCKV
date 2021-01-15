# xml4jsondicom
## DICOM dataset in contextualizedKey-values xml representation

Our design of this XML representation of DICOM datasets is constrainted by the following requirements:
- small markup (including shorthands for mono valued attributes)
- usability of new XPath 3.1 functions (in particular on map, but also higher order functions)
- conversion to json with XPath 3.1 xml-to-json function, with target json results conformant to __[JSON schema](https://json-schema.org/specification.html)__ , also found in __[ietf](https://tools.ietf.org/html/draft-handrews-json-schema-02)__
- enable modularity thanks to a root layer of internal and reference to external categories. The syntaxis used in the root layer conforms to:
    - URI resolution scopes and dereferencing in JSON schema
    - __[RFC 6901 JavaScript Object Notation (JSON) Pointer](https://tools.ietf.org/html/rfc6901)__

