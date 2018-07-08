<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns="http://www.w3.org/2005/xpath-functions"
    xpath-default-namespace="http://www.w3.org/2005/xpath-functions"
    exclude-result-prefixes="xs"
    version="3.0">    
    <xsl:mode on-no-match="shallow-copy"/>    
    <xsl:param name="input"/><!-- input.json -->
    <xsl:output method="xml" indent="yes"/>        
    <xsl:template name="xsl:initial-template">
        <xsl:variable name="json-input-as-xml" select="json-to-xml(unparsed-text($input))"/>    
        <xsl:variable name="transformed-xml" as="document-node()">
            <xsl:apply-templates select="$json-input-as-xml"/> 
        </xsl:variable>
        <xsl:copy-of select="$transformed-xml"/>
    </xsl:template>
    
    <!-- recursive delete of the current attribute or item and the ones nested -->
    <xsl:template match="@*|node()" mode="del">
        <xsl:apply-templates select="@*|node()" mode="del"/>
    </xsl:template>
    
    <!-- modifications -->    
    <xsl:template match="map[@key='00080100']">
        
        <!-- adding an attribute before the matching attribute or item -->
        <map>
            <!-- same tag as nesting sequence -->
            <xsl:attribute name="key" select="../../../@key"/>
            <string key="vr">ii</string>
            <array key="Value">
                <string>oo</string>
            </array>
        </map>
        
        <!-- before matching attribute or item -->
        
        <!-- introspection-->         
        <xsl:variable name="attr" select="."/>    
        <xsl:variable name="tag" select="@key"/>    
        <xsl:variable name="vr" select="string[@vr='vr']/text()"/>    
        <xsl:variable name="stringVal1" select="array/string[1]/text()"/>
        
        
        <!-- recursive copy of current attribute or item and the ones nested -->       
        <xsl:copy>
            <xsl:apply-templates select="@*|node()"/>
        </xsl:copy>
        
        <!-- after matching attribute or item -->
        
    </xsl:template>
        
</xsl:stylesheet>