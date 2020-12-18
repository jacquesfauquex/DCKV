# DICOM dataset in xml representation and corresponding validation

The design of this XML representation of DICOM datasets is constrainted by the following requirements:
- small markup
- easy edition with xslt
- xml schema validation

## small markup
Thanks to contextualizedKey-value syntax, datasets are "flattened" and are represented in uniform two to three level deep xml structures:
- the root level is the element dataset
- DICOM attributes named "a" and corresponponding XML attributes @b (branch) @t (tagchain) and @r (value representation) are listed within the dataset
- when a DICOM attribute contains one or more values, each of these is instantiated by an element named after the value representation inside the attribute. This third level doesn´t appear in case of empty DICOM attributes.

## easy edition with xslt
The XML attributes @b and @t of the contextualizedKey-value syntax were designed to facilitate :
- XPath selection of an attribute or range of attributes, encapsulated into items of sequences or not
- clasification of the attributes

In the XML lenguage, there is no problem with the variability of the value names inside the attributes. "/*" means all values inside the attribute. "/*[2]" means the second value only.

## xml schema validation
At the moment we validate that :
- the values contained in an attribute correspond to the @r
- the text() of the values respect the XML datatype defined to mimic closely the value representation defined in the DICOM standard part 5

xmldicom.xsd is available online from this github repository.
an xml instance, like xmldicom.xml here or any other one, downloads automatically the validating xsd. 

Alternatively, a local copy of the xsd can be referred to in the xml instance with a new path instead of the one in bold blue below:

xsi:schemaLocation="xmldicom.xsd **https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/xml/xmldicom.xsd**"
