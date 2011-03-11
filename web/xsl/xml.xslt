<?xml version="1.0" encoding="UTF-8"?>
<!-- Author: University of Salento and CMCC -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" omit-xml-declaration="no" />

	<xsl:template match="/">
		<xsl:apply-templates />    <!-- Richiama e applica gli altri templates -->
	</xsl:template>
	
	<xsl:template match="*|@*|text()">
		<xsl:copy>
			<xsl:apply-templates select="*|@*|text()" />
		</xsl:copy>
	</xsl:template>
</xsl:stylesheet>
