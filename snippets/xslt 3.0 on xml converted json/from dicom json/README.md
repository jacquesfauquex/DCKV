# json xml converted from dicom json

A "application/dicom+json" response is an array of datasets.
When converted to xml the array of json objects is searchable by means of XPath applied to xml.

Here is the basic syntax of these Xpath functions.

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
