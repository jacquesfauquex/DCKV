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
            <xsl:variable name="table" select="chapter[@label = '7']/table/tbody"/>
            
            <array>
                <xsl:apply-templates select="$table/tr/td[1]/para" mode="index"/>                
            </array>
            <array>
                <xsl:apply-templates select="$table/tr/td[1]/para" mode="tag"/>                
            </array>
            <array>
                <xsl:apply-templates select="$table/tr/td[2]/para"/>                
            </array>
            <array>
                <xsl:apply-templates select="$table/tr/td[3]/para"/>                
            </array>
            <array>
                <xsl:apply-templates select="$table/tr/td[4]/para"/>                
            </array>
            <array>
                <xsl:apply-templates select="$table/tr/td[5]/para"/>                
            </array>
            <array>
                <xsl:apply-templates select="$table/tr/td[6]/para"/>                
            </array>
            
        </array></plist>
    </xsl:template>
    
    <xsl:template match="para" mode="index">
        <integer>
            <xsl:value-of select="tag:hexToDec(tag:plaintext(.))"/>            
        </integer>            
    </xsl:template>
    
    <xsl:template match="para" mode="tag">
        <string>
            <xsl:value-of select="tag:plaintext(.)"/>            
        </string>
    </xsl:template>
    
    <xsl:template match="para">
        <string><xsl:value-of select="normalize-space(.)"/></string>
    </xsl:template>
    
    <xsl:function name="tag:plaintext">
        <xsl:param name="cell"/>
        <xsl:variable name="tag">            
            <xsl:choose>
                <xsl:when test="$cell/emphasis">
                    <xsl:value-of select="$cell/emphasis/text()"/>
                </xsl:when>
                <xsl:when test="$cell/text()">
                    <xsl:value-of select="$cell/text()"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="''"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        <xsl:if test="string-length($tag)=11">
            <xsl:value-of select="concat(substring($tag,2,4),substring($tag,7,4))"/>            
        </xsl:if>
    </xsl:function>
    
    <xsl:function name="tag:hexToDec">
        <xsl:param name="hex"/>
        <xsl:variable name="dec"
            select="string-length(substring-before('0123456789ABCDEF', substring($hex,1,1)))"/>
        <xsl:choose>
            <xsl:when test="matches($hex, '([0-9]*|[A-F]*)')">
                <xsl:value-of
                    select="if ($hex = '') then 0
                    else xs:decimal($dec * tag:power(16, string-length($hex) - 1) + tag:hexToDec(substring($hex,2)))"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message>Provided value is not hexadecimal...</xsl:message>
                <xsl:value-of select="$hex"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>
    
    <xsl:function name="tag:power">
        <xsl:param name="base"/>
        <xsl:param name="exp"/>
        <xsl:sequence
            select="if ($exp lt 0) then tag:power(1.0 div $base, -$exp)
            else if ($exp eq 0)
            then 1e0
            else $base * tag:power($base, $exp - 1)"
        />
    </xsl:function>
</xsl:stylesheet>