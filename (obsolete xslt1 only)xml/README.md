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
xsl 1 is embedded in most modern web browsers, and many opensource tools exist. As a proof of concept of the versality of the contextualizedKey-values xml syntax, transformers were written :
- to text-based json 
- to xml dicom native
- from xml dicom native


## easy edition with xslt
XML native dicom is easy to explore in xsl. XPath selects individualy any elements of its structure.

But there are also limitations:
- the XPath may be very long, especially for attributes in the depth of various levels of items.
- adding or removing an item or a value may imply a fastidious renumbering of the siblings
- because of the complexity of the XPaths (multiple atribute filters at different levels), selections are performed slowly
- the construction of the XML DOM and use of XPath are imperative. Precise hacking, for instance with regex on a flat file, is impossible if the targe is an attribute which is found at various levels (for instance a code).

ContextualizedKey-values xmldicom does not suffer from these limitations as much as xml native dicom:
-the XPath is very short because of the two or three levels máx to reach any attribute and the short names a,b,t,r.
- adding or removing an item or a value does not affect any of the siblings
- XPath is very fast
- XML DOM construction is not imperative. Precise hacking with regex on a flat file is posible.