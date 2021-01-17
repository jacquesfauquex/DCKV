<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns="http://www.w3.org/2005/xpath-functions"
    exclude-result-prefixes="#all"
    expand-text="yes"
    version="3.0">
    
    <xsl:output method="text"/>
    
    <xsl:template match="/">
        <xsl:value-of select="xml-to-json(., map { 'indent' : false() })"/>
    </xsl:template>
    
</xsl:stylesheet>