# xml4jsondicom: DICOM dataset in contextualizedKey-values xml representation

Our design of this XML representation of DICOM datasets is constrainted by the following requirements:
- small markup (including shorthands for mono valued attributes)
- usability of new XPath 3.1 functions (in particular on map, but also higher order functions)
- conversion to json with XPath 3.1 xml-to-json function
- enable modularity thanks to a root layer of internal and reference to external categories

