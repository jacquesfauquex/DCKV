# json xml converted from dicom json

An "application/dicom+json" response is made of an array of objects.

When converted to xml, the array is searchable by means of XPath applied to the xml conversion.

Here are some samples of these XPaths.

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
### Value
```
/map[1]/map[@key='00186011']/array
```
### first string of Value
```
/map[1]/map[@key='00186011']/array/string[1]/text()
```
### first number of Value
```
/map[1]/map[@key='00186011']/array/number[1]/text()
```

## nested attribute in the first item of a sequence attribute of the first dataset
```
/map[1]/map[@key='00186011']/array/map[1]/map[@key='00186012']
```
