# json xml converted from dicom json

A "application/dicom+json" response is an array of datasets.

When converted to xml the array of json objects is searchable by means of XPath applied to xml.

- jxj.xsl in this folder shows how to transform dicom.json to its xml representation (not to dicom.xml) and then back to a dicom.json identical to the original. The comments included may be usefull for whoever wants to try that by her/himself.
- jxt.xsl in this folder uses the transformation a xml and than show how to perform the transformations delete and set on nodes. Adding the final transformation xml to json we would get a dicom.json modified. Bat in this case we left the output in xml. Other outputs are posible too.

The magic of these stylesheet resides in the use of recursive identity transform ( https://en.wikipedia.org/wiki/Identity_transform ) and the lenguaje XPath for tree navigation. Here is the basic XPath syntax that can be used on dicom.json xml transformed.

## non nested attribute of the first dataset
```
/fn:map[1]/fn:map[@key='00186011']
```
If xpath-default-namespace="http://www.w3.org/2005/xpath-functions" was stated in a parent node, this can be simplified as:

```
/map[1]/map[@key='00186011']
```
### tag
```
/map[1]/map[@key='00186011']/@key
```
### vr
```
/map[1]/map[@key='00186011']/string[@key='vr']/text()
```
The vr string requires its key to be explicitly stated because string is also used for InlineBinary an BulkDataURI.

### Value
```
/map[1]/map[@key='00186011']/array
```
array does not require an index because there is always one and only one array inside Value.
### first string of Value
```
/map[1]/map[@key='00186011']/array/string[1]/text()
```
### first number of Value
```
/map[1]/map[@key='00186011']/array/number[1]/text()
```

## nested attribute
```
/map[1]/map[@key='00186011']/array/map[1]/map[@key='00186012']
```
