# mapxmldicom: DICOM dataset in contextualizedKey-values xml map representation

Our design of this XML representation of DICOM datasets is constrainted by the following requirements:
- small markup (shorthand for mono valued attributes)
- markup for SQ end, item start and end, in order to simplify serialization to binary dicom
- conversion to json with XPath 3.1 xml-to-json function
- usability of new XPath 3.1 functions (in particular on map, but also higher order functions) on the dataset
- modularity and possible reference to the modules definitions in the DICOM standard part 3 using json schema markup


## implementation details

__[Subset of map and array functions implementation in XPath 3.1](https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/mapxmldicom/mapxmldicom.xsd)__:
- @key is specialized
- recursion  and contents are restricted



```
<map0>
  |
  +-----------+-----------+-----------+-----------+
  |           |           |           |           |
<map1>     <string1>    <true1>    <false1>    <number1>
  |      
  +-----------+----------+
  |           |          |
<array2>   <null2>    <string2>
  |
  +-----------+----------+
  |           |          |  
<map3>    <string3>   <number3>
  |
  |
  |
<string4>


+---------+--------+---------------+------------------------------------------------------------------+
| map0    | 1      | <map>         | root in targetNamespace="http://www.w3.org/2005/xpath-functions" |
| string1 | [0..n] | <string @key> | reserved for json $id, $schema and $ref properties               |
| map1    | [0..n] | <map @key>    | datasets (for instance one per module)                           |
| true1   | [0..1] | <true @key>   | IOCM marked up. Keep                                             |
| false1  | [0..1] | <false @key>  | IOCM marked up. Reject                                           |
| array1  | [0..n] | <array @key>  | list of ids of child elements (for instances series of a study   |
| number1 | [0..n] | <map @key>    | number of descendant sop instances                               |
| array2  | |0..n] | <array @key>  | attributes                                                       |
| null2   | |0..n] | <null @key>   | end SQ, start and end item                                       |
| string2 | [0..n] | <string @key> | reserved for json $id, $schema and $ref properties               |
| map3    | [0..n] | <map>         | list of references                                               |
| string3 | [0..n] | <string>      | string and base 64 encoded binary attributes values              |
| number3 | [0..n] | <number>      | numeric attributes values                                        |
| string4 | [0..n] | <string @key> | identified urls to resources                                     |
+---------+--------+--------+------+------------------------------------------------------------------+

```

the contents of ```<array2>``` is homogeneous ( ```<map3>``` only or ```<string3>``` only or ```<number3>``` only )
