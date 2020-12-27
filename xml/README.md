# DICOM dataset in contextualizedKey-values xml representation

Our design of this XML representation of DICOM datasets is constrainted by the following requirements:
- small markup
- xml schema validation
- conversion to other representations with xsl 1
- easy edition with xsl 1

## small markup
Using the contextualizedKey-values syntax, datasets are "flattened" and represented in uniform two-to-three-level-deep xml structures:
- the root level is the element ```<dataset>```
- DICOM attributes named ```<a>``` and corresponponding XML attributes ```@b``` (branch) ```@t``` (tagchain) and ```@r``` (value representation) are listed as XML attributes
- Each value of the DICOM attribute is an element named after the value representation. For instance ```<CS>```.This third level doesn´t appear in case of empty DICOM attributes.

## xml schema validation
In XML tools where schema validation is enabled, __[our validating XML schema](https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/xml/xmldicom.xsd)__ checks these aspects of well-formedness:
- the values contained in an attribute correspond to the value representation @r
- the text() of the values respect quite closely the value representation defined in the DICOM standard part 5

Any XML dataset starting with a root element like __[xmldicom.xml](https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/xml/xmldicom.xml)__ downloads automatically the validating xsd. 

Alternatively, a local copy of the xsd can be referred to in the xml instance with a new path instead of the one in bold blue below:

xsi:schemaLocation="xmldicom.xsd **https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/xml/xmldicom.xsd**"

We believe that specific SOP classes may be further validated with specialization of this schema which formalize the structures of modules of information based on the part 3 of the DICOM standard.

## conversion to other representations with xsl 1
xsl 1 is embedded in most modern web browsers, and many opensource tools exist for its application. As a proof of concept of the versality of the contextualizedKey-values xml syntax, transformers were written to text-based json and to xml dicom native format.


## easy edition with xslt

