<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns="http://www.w3.org/2005/xpath-functions"
    xpath-default-namespace="http://www.w3.org/2005/xpath-functions"
    exclude-result-prefixes="xs"
    version="3.0">
    
    <xsl:mode on-no-match="shallow-copy"/>
    <!-- 
        identity transform template 
        applying a copy any time an attribute is not intercepted
    -->
    
    <xsl:param name="input"/>
    <!-- ../../input.json -->

    <xsl:output method="text" media-type="text/json"/>
    <!-- 
        do not use method json, because results as a whole
        is treated as one string,
        implying that the string inside it have their " escaped
    -->
        
    <xsl:template name="xsl:initial-template">
        <!-- This name tells saxon 9.8 where to start -->
        
        <xsl:variable name="json-input-as-xml" select="json-to-xml(unparsed-text($input))"/>    
        <xsl:variable name="transformed-xml" as="document-node()">
            <xsl:apply-templates select="$json-input-as-xml"/>
            <!-- 
                this allows to intercept any attribute located anywhere 
                and delete it or modify it or add other attributes before or after it. 
                Look at jx for examples
            -->
        </xsl:variable>
        <xsl:value-of select="xml-to-json($transformed-xml, map { 'indent' : true() })"/>
    </xsl:template>
    
</xsl:stylesheet>