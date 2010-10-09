<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <!-- Include other stylesheets -->
  <xsl:import href="docbook-xsl:/html/docbook.xsl"/>
  <xsl:include href="../common/params.xsl" />
  <xsl:include href="../common/inline.xsl" />
  <xsl:output method="html" encoding="UTF-8" indent="no"/>

  <xsl:param name="html.stylesheet" select="'documentation.css'"/>
  <xsl:param name="use.extensions" select="0"/>
  <xsl:param name="section.autolabel" select="'1'"/>

  <xsl:param name="generate.toc">
    appendix  nop
    article/appendix  nop
    article   toc,title
    book      toc,title
    chapter   nop
    part      nop
    preface   nop
    qandadiv  nop
    qandaset  nop
    reference nop
    sect1     nop
    sect2     nop
    sect3     nop
    sect4     nop
    sect5     nop
    section   nop
    set       nop
  </xsl:param>

</xsl:stylesheet> 

