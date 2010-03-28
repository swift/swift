<?xml version='1.0'?> 
<xsl:stylesheet 
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0">

  <xsl:template match="application">
    <fo:inline font-style="italic"><xsl:call-template name="inline.charseq"/></fo:inline>
  </xsl:template>

  <xsl:template match="guimenu">
    <fo:inline font-style="italic"><xsl:call-template name="inline.charseq"/></fo:inline>
  </xsl:template>

  <xsl:template match="guilabel">
    <fo:inline font-style="italic"><xsl:call-template name="inline.charseq"/></fo:inline>
  </xsl:template>

  <xsl:template match="guimenuitem">
    <fo:inline font-style="italic"><xsl:call-template name="inline.charseq"/></fo:inline>
  </xsl:template>

  <xsl:template match="guisubmenu">
    <fo:inline font-style="italic"><xsl:call-template name="inline.charseq"/></fo:inline>
  </xsl:template>

</xsl:stylesheet>
