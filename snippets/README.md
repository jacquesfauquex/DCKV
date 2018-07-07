# XSLT 3.0 JSON

This folder contains XSLT transformations applied to JSON input.

In XSLT it is posible to:

- (a) manage json input transformed to xml
- (b) manage json input directly using XPath functions

(https://www.saxonica.com/papers/xmlprague-2016mhk.pdf)

We design proofs of concept of these two options applied to:

- (1) dicom json source
- (2) dicom APath json source

The first objective is to compare the legibility of the various snippets.

In a second step, will include benchmarking of the various combinations.

All the snippets are edited with Oxygenxml 20.1 using saxon EE 9.8.0.12.
