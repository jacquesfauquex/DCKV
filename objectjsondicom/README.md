# objectjsondicom
## DICOM dataset contextualizedKey-values representation as JSON

DICOM dataset contextualizedKey-values representation may be written as XML or as JSON. Two functions of XPath3.1 lenguage xml-to-json() and json2xml() are instrumenting the equivalency.

JSON is more compact and since the creation of json-schema, there is a standard way to link external objects json objects. This seems to be an interesting feature for the development of a database of DICOM dataset objects.

On the xml side there exists __[Xlink](https://www.w3.org/TR/xlink11/)__. We found dificult to adapt it to our representation of dicom dataset because the specification requires its own xml namespace. This burden is not found in the json syntax to link objects.

In the xml schema of the representation, we intentionally added ```<string>``` elements within the root ```<map>``` (map0) and the dicom attributes ```<map>``` (map1) in order to incorporate json markup around the core dicom dataset and posibly link JSON objects.

## JSON schema (draft 7) and JSON pointers and external links
- https://json-schema.org/learn/getting-started-step-by-step.html
- https://json-schema.org/learn/file-system.html
- https://json-schema.org/understanding-json-schema/


