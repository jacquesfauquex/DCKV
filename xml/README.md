# DICOM dataset in contextualizedKey-values xml representation

Our design of this XML representation of DICOM datasets is constrainted by the following requirements:
- small markup
- xml schema validation
- conversion to other representations with xsl 1
- easy edition with xsl 1

## small markup
Thanks to contextualizedKey-values syntax, datasets are "flattened" and represented in uniform two-to-three-level-deep xml structures:
- the root level is the element dataset
- DICOM attributes named "a" and corresponponding XML attributes @b (branch) @t (tagchain) and @r (value representation) are listed as XML attributes
- Each value of the DICOM attribute is an element named after the value representation. This third level doesn´t appear in case of empty DICOM attributes.

## xml schema validation
In XML tools where schema validation is enabled, __[our validating XML schema](https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/xml/xmldicom.xsd)__ checks these aspects of well-formedness:
- the values contained in an attribute correspond to the value representation @r
- the text() of the values respect quite closely the value representation defined in the DICOM standard part 5

Any XML dataset starting with a root element like __[xmldicom.xml](https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/xml/xmldicom.xml)__ downloads automatically the validating xsd. 

Alternatively, a local copy of the xsd can be referred to in the xml instance with a new path instead of the one in bold blue below:

xsi:schemaLocation="xmldicom.xsd **https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/xml/xmldicom.xsd**"

## conversion to other representations with xsl 1


## easy edition with xslt

