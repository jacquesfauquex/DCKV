# xml4jsondicom: DICOM dataset in contextualizedKey-values xml representation

Our design of this XML representation of DICOM datasets is constrainted by the following requirements:
- small markup (shorthand for mono valued attributes)
- markup for SQ end, item start and end, in order to simplify serialization to binary dicom
- conversion to json with XPath 3.1 xml-to-json function
- usability of new XPath 3.1 functions (in particular on map, but also higher order functions) on the dataset
- modularity
- possible reference to the modules definitions in the DICOM standard part 3 


## implementation details

- The root object is a map with targetNamespace="http://www.w3.org/2005/xpath-functions"
- Inside a map 
    - string is reserved for $id, $schema and $ref properties
    - array is reserved of dicom attributes
    - null is reserved for SQ end, item start and end markup
- Inside an array
    - string is reserved for string and base64 binary dicom attributes
    - number is reserved for number dicom attributes
    - map is resereved for url reference to external binary resources
