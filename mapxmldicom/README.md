# mapxmldicom: DICOM dataset in contextualizedKey-values xml map representation

Our design of this XML representation of DICOM datasets is constrainted by the following requirements:
- small markup (shorthand for mono valued attributes)
- markup for SQ end, item start and end, in order to simplify serialization to binary dicom
- conversion to json with XPath 3.1 xml-to-json function
- usability of new XPath 3.1 functions (in particular on map, but also higher order functions) on the dataset
- modularity (e.j. a study mapxmldicom, which lists series mapxmldicom, which lists instance mapxmldicom). Desigend for nosql Mongodb integration.


## implementation details

__[Subset of map and array functions implementation in XPath 3.1](https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/mapxmldicom/mapxmldicom.xsd)__:
- @key is specialized
- recursion  and contents are restricted



```
<map0>
  |
  +-----------+-----------+-----------+-----------+-----------+
  |           |           |           |           |           |
<map1>     <string1>    <true1>    <false1>    <number1>   <array1>
  |      
  +-----------+
  |           |
<array2>   <null2>
  |
  +-----------+----------+
  |           |          |  
<map3>    <string3>   <number3>
  |
  |
  |
<array4>
  |
  |
  |
<string5>



+=========+==========+===============+==============================================================+
| level   | how many | object        | description                                                  |
+=========+==========+===============+==============================================================+
| map0    | 1        | <map>         | root                                                         |
+----------+---------+---------------+--------------------------------------------------------------+
| map1    | [0..n]   | <map @key>    | dataset(s)                                                   |
| string1 | [0..n]   | <string @key> | object id                                                    |
| true1   | [0..1]   | <true @key>   | IOCM keep                                                    |
| false1  | [0..1]   | <false @key>  | IOCM obfuscate                                               |
| number1 | [0..n]   | <map @key>    | number of descendant sop instances                           |
| array1  | [0..n]   | <array @key>  | list of ids of child elements (e.j series of a study module) |
+---------+----------+---------------+--------------------------------------------------------------+
| array2  | |0..n]   | <array @key>  | attributes                                                   |
| null2   | |0..n]   | <null @key>   | end SQ, start and end item                                   |
+---------+----------+---------------+--------------------------------------------------------------+
| map3    | [0..n]   | <map>         | list of references                                           |
| string3 | [0..n]   | <string>      | string and base 64 encoded binary attributes values          |
| number3 | [0..n]   | <number>      | numeric attributes values                                    |
+---------+----------+---------------+--------------------------------------------------------------+
| array4  | [0..n]   | <array @key>  | alternative lists of urls to resources                       |
+---------+----------+--------+------+--------------------------------------------------------------+
| string5 | [0..n]   | <string>      | url(s) to fragments or to one resource                       |
+---------+----------+--------+------+--------------------------------------------------------------+

```
### Notes:
- map0 targetNamespace="http://www.w3.org/2005/xpath-functions"
- the contents of ```<array2>``` must be homogeneous ( ```<map3>``` only or ```<string3>``` only or ```<number3>``` only )


## Write as json

See:
- https://stackoverflow.com/questions/62927952/how-to-convert-xml-to-json-in-xslt
- https://stackoverflow.com/questions/62334578/using-xslt-3-0-to-transform-xml

__[M2J.xsl](https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/mapxmldicom/M2J.xsl)__ shows how to write univocally this XML as JSON, using the XSLT 3.0 function *xml-to-json()*
