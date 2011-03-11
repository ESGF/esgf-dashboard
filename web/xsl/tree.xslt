<?xml version="1.0" encoding="UTF-8"?>
<!-- Author: University of Salento and CMCC -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" omit-xml-declaration="yes" />

	<xsl:template match="/">
		[<xsl:apply-templates />    <!-- Richiama e applica gli altri templates %3Cbr%3E -->]
	</xsl:template>
	
	<xsl:template match="*|@*">		
		{
			text: "&lt;<xsl:value-of select="name()" />&gt;",
			iconCls: "treeXML",
			children: [<xsl:apply-templates />]
		},
		{
			text: "&lt;/<xsl:value-of select="name()" />&gt;", iconCls: "treeXML", leaf: true
		}<xsl:if test="position()!=last()">,</xsl:if>
	</xsl:template>
	
	<xsl:template match="text()">
		{text: "<xsl:value-of select="." />", iconCls: "treeXML", leaf: true}
	</xsl:template>	
</xsl:stylesheet>
