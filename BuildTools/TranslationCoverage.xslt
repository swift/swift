<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!--

To use this XSLT, run it with xsltproc:

    xsltproc BuildTools/TranslationCoverage.xslt Swift/Translations/swift_nl.ts

Supported parameters:

  full      if set to 1 or true, generate a full report instead of the default oneliner.

-->

<xsl:param name="full" select="'0'" />
<xsl:param name="filename" />
<xsl:output method="text" />

<xsl:variable name="contexts" select="count(TS/context)"/>
<xsl:variable name="vanished" select="count(TS/context/message/translation[@type = 'vanished'])"/>
<xsl:variable name="obsolete" select="count(TS/context/message/translation[@type = 'obsolete'])"/>
<xsl:variable name="strings" select="count(TS/context/message/source) - $vanished - $obsolete"/>
<xsl:variable name="translations" select="count(TS/context/message/translation[not(@type) and text()])"/>
<xsl:variable name="missing" select="$strings - $translations"/>
<xsl:variable name="percent_done" select="floor($translations div $strings * 100)"/>

<xsl:template match="/TS">
	<xsl:choose>
		<xsl:when test="$full != '0' and $full != 'false'"><xsl:call-template name="full" /></xsl:when>
		<xsl:otherwise><xsl:call-template name="terse" /></xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="terse"><xsl:value-of select="$percent_done"/>% complete, <xsl:value-of select="$vanished"/> vanished, <xsl:value-of select="$obsolete"/> obsolete, <xsl:value-of select="$missing"/> missing
</xsl:template>

<xsl:template name="full"><xsl:if test="$filename">Report for <xsl:value-of select="$filename" />:

</xsl:if>Contexts:       <xsl:value-of select="$contexts"/>
Active Strings: <xsl:value-of select="$strings"/>
Translations:   <xsl:value-of select="$translations"/>, <xsl:value-of select="$percent_done"/>% complete
Missing:        <xsl:value-of select="$missing"/>
Vanished:       <xsl:value-of select="$vanished"/>
Obsolete:       <xsl:value-of select="$obsolete"/>

Note: Strings and Translations do not include vanished or obsolete messages.
</xsl:template>

</xsl:stylesheet>
