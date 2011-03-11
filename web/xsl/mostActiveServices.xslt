<?xml version="1.0" encoding="UTF-8"?>
<!-- Author: University of Salento and CMCC -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" omit-xml-declaration="no" indent="yes" />
	<xsl:template match="/result/item">
		<results>
			<xsl:apply-templates select="services" />			
			<xsl:copy-of select="totalCount" />
		</results>
	</xsl:template>
	
	<xsl:template match="/result/item/services">
		<xsl:copy>
			<xsl:apply-templates select="item" />
		</xsl:copy>	
	</xsl:template>
	
	<xsl:template match="/result/item/services/item">
		<xsl:copy>
			<xsl:copy-of select="id" />
			<xsl:copy-of select="name" />
			<xsl:copy-of select="port" />
			<xsl:apply-templates select="percentages" />
		</xsl:copy>
	</xsl:template>
	
	<xsl:template match="/result/item/services/item/percentages">
		<xsl:call-template name="itemTemplate">
			<xsl:with-param name="SPAN">5 minutes</xsl:with-param>
			<xsl:with-param name="I" select="1" />
		</xsl:call-template>
		<xsl:call-template name="itemTemplate">
			<xsl:with-param name="SPAN">1 hour</xsl:with-param>
			<xsl:with-param name="I" select="2" />
		</xsl:call-template>
		<xsl:call-template name="itemTemplate">
			<xsl:with-param name="SPAN">1 day</xsl:with-param>
			<xsl:with-param name="I" select="3" />
		</xsl:call-template>
		<xsl:call-template name="itemTemplate">
			<xsl:with-param name="SPAN">1 week</xsl:with-param>
			<xsl:with-param name="I" select="4" />
		</xsl:call-template>
		<xsl:call-template name="itemTemplate">
			<xsl:with-param name="SPAN">1 month</xsl:with-param>
			<xsl:with-param name="I" select="5" />
		</xsl:call-template>
	</xsl:template>
	
	<xsl:template name="itemTemplate">
		<xsl:param name="SPAN" select="0" />
		<xsl:param name="I" />
		<activity>
			<timeSpan><xsl:value-of select="$SPAN" /></timeSpan>
			<value><xsl:value-of select="./item[$I]" /></value>
		</activity>
	</xsl:template>
</xsl:stylesheet>