<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns="http://www.w3.org/2005/xpath-functions"
    xpath-default-namespace="http://www.w3.org/2005/xpath-functions"
    exclude-result-prefixes="xs"
    version="3.0">    
    <xsl:mode on-no-match="shallow-copy"/>    
    <xsl:param name="input"/><!-- ../input.json -->
    <xsl:output method="xml" indent="yes"/>        
    <xsl:template name="xsl:initial-template">
        <xsl:variable name="json-input-as-xml" select="json-to-xml(unparsed-text($input))"/>    
        <xsl:variable name="transformed-xml" as="document-node()">
            <xsl:apply-templates select="$json-input-as-xml"/> 
        </xsl:variable>
        <xsl:copy-of select="$transformed-xml"/>
    </xsl:template>
    
    <!-- functions -->
    
    <!-- delete current and nodes nested-->
    <xsl:template match="@*|node()" mode="del">
        <xsl:apply-templates select="@*|node()" mode="del"/>
    </xsl:template>
    
    <xsl:template name="set-attr">
        <xsl:param name="node"/>
        <xsl:param name="key"/>
        <xsl:param name="vr"/>
        <xsl:param name="string"/>
        <xsl:map-entry key="$key" select="map { 'vr' : $vr , 'Value' : [ $string ] }"/>
    </xsl:template>
    
    <!-- editions -->
    
    <xsl:template match="map[@key='00080100']">
        <map>
            <xsl:attribute name="key" select="../../../@key"/>
            <string key="vr">ii</string>
            <array key="Value">
                <string>oo</string>
            </array>
        </map>
        <map key="00000000">
            <string key="vr">xx</string>
            <array key="Value">
                <string>yy</string>
            </array>
        </map>
    </xsl:template>
        <!--
             select="map { 'vr' : 'map('vr')' , 'Value' : [ $string ] }"
            
            <xsl:call-template name="del-attr"/>
        <xsl:call-template name="set-attr">
            <xsl:with-param name="node" select="."/>
            <xsl:with-param name="key" select="@key"/>
            <xsl:with-param name="vr" select="@vr"/>
            <xsl:with-param name="string" select="text()"/>
        </xsl:call-template>-->
    
    
    <xsl:template match="map[@key='00020001']/*">
        <xsl:choose>
            <xsl:when test="@key='vr'">
                <!-- <xsl:copy-of select="."/> -->
                <string key="vr">rr</string>
            </xsl:when>
            <xsl:when test="@key='Value'">
                <xsl:copy-of select="."/>
            </xsl:when>
            <xsl:when test="@key='InlineBinary'">
                <xsl:copy-of select="."/>
            </xsl:when>
            <xsl:when test="@key='BulkDataURI'">
                <xsl:copy-of select="."/>
            </xsl:when>
            <xsl:otherwise>
                <!-- remove -->
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!-- delete 
    <xsl:template match="map[@key='00089215']">
        <xsl:apply-templates select="@*|node()" mode="del"/>
    </xsl:template>
    -->
    
</xsl:stylesheet>