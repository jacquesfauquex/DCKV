<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet  
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:dx="xmldicom.xsd" 
    xsi:schemaLocation="xmldicom.xsd 
    https://raw.githubusercontent.com/jacquesfauquex/DICOM_contextualizedKey-values/master/xml/xmldicom.xsd"
    xml:space="default"
    >
    <xsl:output method="xml" />
    <!-- ==================================================== -->
    <!-- creates dicom xml from contextualized key values xml -->
    <!-- ==================================================== -->
    

    <!-- string functions -->
    
    <xsl:template name="extensionNumber">
        <xsl:param name="dotString"/>
        <xsl:choose>
            <xsl:when test="contains($dotString, '.')">
                <xsl:call-template name="extensionNumber">
                    <xsl:with-param name="dotString" select="substring-after($dotString, '.')"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="number($dotString)"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template name="removeExtension">
        <xsl:param name="dotString"/>
        <xsl:param name="first" select="true()"/>
        <xsl:if test="contains($dotString, '.')">
            <xsl:if test="not($first)">
                <xsl:text>.</xsl:text>
            </xsl:if>
            <xsl:value-of select="substring-before($dotString, '.')"/>
            <xsl:call-template name="removeExtension">
                <xsl:with-param name="dotString" select="substring-after($dotString, '.')"/>
                <xsl:with-param name="first" select="false()"/>
            </xsl:call-template>
        </xsl:if>
    </xsl:template>
       
    <!-- ordered list -->
    <xsl:variable name="tagBranchList">
        <xsl:for-each select="/dx:dataset/dx:a">
            <xsl:element name="dx:a">
                <xsl:copy-of select="@b"/>
                <xsl:copy-of select="@t"/>
                <xsl:copy-of select="@r"/>
                <xsl:attribute name="tb">
                    <xsl:call-template name="tagBranch">
                        <xsl:with-param name="t" select="@t"/>
                        <xsl:with-param name="b" select="@b"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:copy-of select="*"/>
            </xsl:element>
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
    
    <xsl:variable name="orderedlist">
        <xsl:for-each select="$tagBranchList/dx:a">
            <xsl:sort select="@tb"/>
            <xsl:copy-of select="."/>
        </xsl:for-each>
    </xsl:variable>
    
    <xsl:variable name="list">
        <xsl:for-each select="$orderedlist/dx:a">
            <xsl:element name="dx:a">
                <xsl:copy-of select="@b"/><!-- branch -->
                <xsl:copy-of select="@t"/><!-- tagchain -->
                <xsl:copy-of select="@r"/><!-- vr -->
                <xsl:copy-of select="@tb"/><!-- clasifier -->
                <xsl:attribute name="p"><!-- parent -->
                    <xsl:call-template name="removeExtension">
                        <xsl:with-param name="dotString" select="@tb"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="i"><!-- item -->
                    <xsl:call-template name="extensionNumber">
                        <xsl:with-param name="dotString" select="@b"/>
                    </xsl:call-template>
                </xsl:attribute>
                <xsl:attribute name="l"><!-- level 0,1,...-->
                    <xsl:value-of select="string-length(@b) - string-length(translate(@b, '.',''))"/>
                </xsl:attribute>
                <xsl:copy-of select="*"/>
            </xsl:element>
        </xsl:for-each>
    </xsl:variable>


    <!-- root template -->
    
    <xsl:template match="/dx:dataset">
        <NativeDicomModel>
            <xsl:apply-templates select="$list/dx:a[@l=0]" mode="dataset"/>
        </NativeDicomModel>    
    </xsl:template>
    
    
    <!-- attributes, items and values -->
    
    <xsl:template match="dx:a" mode="dataset">
        <xsl:element name="DicomAttribute">
            <xsl:attribute name="tag">
                <xsl:value-of select="substring(@t, string-length(@t) - 7, 8)"/>
            </xsl:attribute>
            <xsl:attribute name="vr">
                <xsl:value-of select="@r"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="@r='SQ'">
                    <xsl:variable name="SQrootTb" select="@tb"/>
                    <xsl:variable name="SQelements" select="$list/dx:a[@p=$SQrootTb]"/>
                    <xsl:apply-templates select="$SQelements[(position()=1) or (not(@p=preceding-sibling::a[1]/@p))]" mode="item">
                        <xsl:with-param name="SQelements" select="$SQelements"/>
                    </xsl:apply-templates>                         
                </xsl:when>
                <xsl:otherwise>
                    <xsl:apply-templates select="*"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
    
    <xsl:template match="dx:a" mode="item"><!-- Item -->
        <xsl:param name="SQelements"/>
        <xsl:variable name="itemIndex" select="@i"/>
        <xsl:element name="Item">
            <xsl:attribute name="number"><xsl:value-of select="position()"/></xsl:attribute>            
            <xsl:if test="substring(@t, string-length(@t), 1) != '.'">
                <xsl:apply-templates select="$SQelements[@i=$itemIndex]" mode="dataset"/>
            </xsl:if>
        </xsl:element>
    </xsl:template>
    

    <xsl:template match="dx:AE|dx:AS|dx:AT|dx:CS|dx:DA|dx:DS|dx:DT|dx:IS|dx:LO|dx:LT|dx:SH|dx:ST|dx:SV|dx:TM|dx:UC|dx:UI|dx:UR|dx:UT|dx:UV"><!-- string -->
        <xsl:element name="Value">
            <xsl:attribute name="number"><xsl:value-of select="position()"/></xsl:attribute>
            <xsl:copy-of select="text()"/>
        </xsl:element>
    </xsl:template>
    
    <xsl:template match="dx:FL|dx:FD|dx:SL|dx:SS|dx:UL|dx:US"><!-- number -->
        <xsl:element name="Value">
            <xsl:attribute name="number"><xsl:value-of select="position()"/></xsl:attribute>
            <xsl:copy-of select="text()"/>
        </xsl:element>
    </xsl:template>
    
    <xsl:template match="dx:OB|dx:OD|dx:OF|dx:OL|dx:OV|dx:OW|dx:UN"><!-- base64 -->
        <xsl:element name="Value">
            <xsl:attribute name="number"><xsl:value-of select="position()"/></xsl:attribute>
            <xsl:copy-of select="text()"/>
        </xsl:element>
    </xsl:template>
    
    <xsl:template match="dx:PN"><!-- person name -->
        <xsl:element name="PersonName">
            <xsl:attribute name="number"><xsl:value-of select="position()"/></xsl:attribute>

                <xsl:variable name="alphabetic" select="substring-before(.,'=')"/> 
                <xsl:if test="string-length(translate($alphabetic,'^','')) > 0">
                    <xsl:element name="Alphabetic">
                        <xsl:call-template name="nameComponents">
                            <xsl:with-param name="name" select="$alphabetic"/>
                        </xsl:call-template>
                    </xsl:element>
                </xsl:if>
                
                <xsl:variable name="nonAlphabetic" select="substring-after(.,'=')"/>   
                <xsl:if test="string-length(.) != string-length($nonAlphabetic)"> 
                    
                    <xsl:variable name="ideographic" select="substring-before($nonAlphabetic,'=')"/>                                
                    <xsl:if test="string-length(translate($ideographic,'^','')) > 0">
                        <xsl:element name="Ideographic">
                            <xsl:call-template name="nameComponents">
                                <xsl:with-param name="name" select="$ideographic"/>
                            </xsl:call-template>                    
                        </xsl:element>
                    </xsl:if>
                    
                    <xsl:variable name="phonetic" select="substring-after($nonAlphabetic,'=')"/>   
                    <xsl:if test="string-length(translate($phonetic,'^','')) > 0">
                        <xsl:element name="Phonetic">
                            <xsl:call-template name="nameComponents">
                                <xsl:with-param name="name" select="$phonetic"/>
                            </xsl:call-template>                    
                        </xsl:element>
                    </xsl:if>                    
               </xsl:if>
        </xsl:element>
    </xsl:template>

    <xsl:template name="nameComponents">
        <xsl:param name="name"/>
        
        <xsl:variable name="familyName" select="substring-before($name,'^')"/>                
        <xsl:if test="string-length($familyName) > 0">
            <xsl:element name="FamilyName">
                <xsl:value-of select="$familyName"/>
            </xsl:element>            
        </xsl:if>
        
        <xsl:variable name="noFamilyName" select="substring-after($name,'^')"/>  
        <xsl:if test="string-length($noFamilyName) != string-length($name)">
        
            <xsl:variable name="givenName" select="substring-before($noFamilyName,'^')"/>                
            <xsl:if test="string-length($givenName) > 0">
                <xsl:element name="GivenName">
                    <xsl:value-of select="$givenName"/>
                </xsl:element>            
            </xsl:if>
            
            <xsl:variable name="noGivenName" select="substring-after($noFamilyName,'^')"/>  
            <xsl:if test="string-length($noGivenName) != string-length($noFamilyName)">
                               
                <xsl:variable name="middleName" select="substring-before($noGivenName,'^')"/>                
                <xsl:if test="string-length($middleName) > 0">
                    <xsl:element name="MiddleName">
                        <xsl:value-of select="$middleName"/>
                    </xsl:element>            
                </xsl:if>
                
                <xsl:variable name="noMiddleName" select="substring-after($noGivenName,'^')"/>  
                <xsl:if test="string-length($noMiddleName) != string-length($noGivenName)">                    
                    
                    <xsl:variable name="namePrefix" select="substring-before($noMiddleName,'^')"/>                
                    <xsl:if test="string-length($namePrefix) > 0">
                        <xsl:element name="NamePrefix">
                            <xsl:value-of select="$namePrefix"/>
                        </xsl:element>            
                    </xsl:if>
                    
                    <xsl:variable name="nameSuffix" select="substring-after($noMiddleName,'^')"/>  
                    <xsl:if test="string-length($nameSuffix) > 0">              
                        <xsl:element name="NameSuffix">
                            <xsl:value-of select="$nameSuffix"/>
                        </xsl:element>
                    </xsl:if>
                </xsl:if>
            </xsl:if>
        </xsl:if>
    </xsl:template>
    
</xsl:stylesheet>