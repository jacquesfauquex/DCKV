<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:dx="xmldicom.xsd" 
    xsi:schemaLocation="xmldicom.xsd https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/xml/xmldicom.xsd">
    <xsl:output method="text"/>
    <!-- https://www.json.org/json-en.html -->
    <!-- creates opendicom json from opendicom xml -->

    <xsl:variable name="tagBranchList">
        <xsl:for-each select="/dx:dataset/dx:a">
            <xsl:element name="a"><xsl:copy-of select="@b"/><xsl:copy-of select="@t"/><xsl:copy-of select="@r"/><xsl:attribute name="tb"><xsl:call-template name="tagBranch"><xsl:with-param name="t" select="@t"/><xsl:with-param name="b" select="@b"/></xsl:call-template></xsl:attribute><xsl:copy-of select="*"/></xsl:element>
       </xsl:for-each>
    </xsl:variable>

    <xsl:template name="tagBranch">
        <xsl:param name="t"/>
        <xsl:param name="b"/>
        <xsl:choose>
            <xsl:when test="contains($t, '.')">
                <xsl:value-of select="concat(substring-before($t, '.'),'-',substring-before($b, '.'),'.')"/>
                <xsl:call-template name="tagBranch">
                   <xsl:with-param name="t" select="substring-after($t, '.')"/>
                    <xsl:with-param name="b" select="substring-after($b, '.')"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="concat($t,'-',$b)"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <xsl:variable name="list">
        <xsl:for-each select="$tagBranchList/*">
            <xsl:sort select="@tb" data-type="text" order="ascending"/>
            <xsl:copy-of select="."/>
        </xsl:for-each>
    </xsl:variable>
    
    <xsl:variable name="afterList" select="count($list/a) + 1"/>
    
    
    <xsl:variable name="number_r" select="'FL|FD|SL|SS|UL|US'"/>
    <xsl:variable name="person_r" select="'PN'"/>
    <xsl:variable name="base64_r" select="'OB|OD|OF|OL|OV|OW|UN'"/>
    <!--
    <xsl:variable name="sting_r" select="'AE|AS|AT|CS|DA|DS|DT|IS|LO|LT|SH|ST|SV|TM|UC|UI|UR|UT|UV'"/>
    <xsl:variable name="sequence_r" select="'SQ'"/>
    -->
    
    <xsl:template match="/dx:dataset">
        <xsl:text>{</xsl:text>
        <xsl:for-each select="$list/a">
            <xsl:if test="position()>1">
                <xsl:text>,</xsl:text>
            </xsl:if>
            <xsl:value-of select="concat(' &quot;',@tb,'~',@r,'&quot; :[')"/>
            <xsl:choose>
                <xsl:when test="contains($number_r , @r)">
                    <xsl:for-each select="*">
                        <xsl:if test="position()>1">
                            <xsl:text>,</xsl:text>
                        </xsl:if>                            
                        <xsl:value-of select="concat(' ',text(),' ')"/>
                    </xsl:for-each>                               
                </xsl:when>
                
                <xsl:when test="contains($person_r,@r)">
                    <xsl:for-each select="*">
                        <xsl:if test="position()>1">
                            <xsl:text>,</xsl:text>
                        </xsl:if>                            
                        <xsl:value-of select="concat(' { &quot;Alphabetic&quot; :&quot;',text(),'&quot; } ')"/>
                    </xsl:for-each>                               
                </xsl:when>
                
                <xsl:when test="contains($base64_r,@r)">
                    <xsl:for-each select="*">
                        <xsl:if test="position()>1">
                            <xsl:text>,</xsl:text>
                        </xsl:if>                            
                        <xsl:value-of select="concat(' &quot;',text(),'&quot; ')"/>
                    </xsl:for-each>                               
                </xsl:when>
                
                <xsl:otherwise><!-- $string_r -->
                    <xsl:for-each select="*">
                        <xsl:if test="position()>1">
                            <xsl:text>,</xsl:text>
                        </xsl:if>                            
                        <xsl:value-of select="concat(' &quot;',text(),'&quot; ')"/>
                    </xsl:for-each>
                </xsl:otherwise>
            </xsl:choose>

            <xsl:value-of select="' ]'"/>
        </xsl:for-each>
        <xsl:text> }</xsl:text>    
    </xsl:template>
    
</xsl:stylesheet>