<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="3.0"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:math="http://www.w3.org/2005/xpath-functions/math"
    xmlns:docbook="http://docbook.org/ns/docbook"
    xpath-default-namespace="http://docbook.org/ns/docbook"
    xmlns:tag="http://www.opendicom.com/xsd/dicom/tag"
    xmlns="http://www.apple.com/DTDs/PropertyList-1.0.dtd"
    exclude-result-prefixes="xs math docbook tag"
    >
    
    <xsl:output indent="yes"/>
    
    <xsl:mode on-no-match="shallow-copy"/>
    
    <xsl:template match="/book">
        <plist version="1.0"><array>
            <xsl:variable name="table881" select="chapter[@label = '8']/table[@xml:id = 'table_8.8-1']/tbody"/>
            <xsl:variable name="table881a" select="chapter[@label = '8']/table[@xml:id = 'table_8.8-1a']/tbody"/>
            <xsl:variable name="table881b" select="chapter[@label = '8']/table[@xml:id = 'table_8.8-1b']/tbody"/>
            
            <array>
                <xsl:apply-templates select="$table881/tr/td[1]/para"/>                
            </array>
            <array>
                <xsl:apply-templates select="$table/tr/td[2]/para"/>                
            </array>
            <array>
                <xsl:apply-templates select="$table/tr/td[3]/para"/>                
            </array>
            
        </array></plist>
    </xsl:template>

    <xsl:template match="para">
        <string><xsl:value-of select="normalize-space(.)"/></string>
    </xsl:template>
    
</xsl:stylesheet>