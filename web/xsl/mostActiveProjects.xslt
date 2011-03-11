<?xml version="1.0" encoding="UTF-8"?>
<!-- Author: University of Salento and CMCC -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" omit-xml-declaration="no" indent="yes" />
	<xsl:template match="/result/item">
		<results>
			<xsl:apply-templates select="projects" />			
			<xsl:copy-of select="totalCount" />
		</results>
	</xsl:template>
	
	<xsl:template match="/result/item/projects">
		<xsl:copy>
			<xsl:apply-templates select="item" />
		</xsl:copy>	
	</xsl:template>
	
	<xsl:template match="/result/item/projects/item">
		<xsl:copy>
			<xsl:copy-of select="id" />
			<xsl:copy-of select="name" />
			<xsl:copy-of select="startDate" />
			<xsl:copy-of select="startTime" />
			<xsl:copy-of select="endDate" />
			<xsl:copy-of select="endTime" />
			<xsl:apply-templates select="percentages" />
		</xsl:copy>
	</xsl:template>
	
	<xsl:template match="/result/item/projects/item/percentages">
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
		<availability>
			<timeSpan><xsl:value-of select="$SPAN" /></timeSpan>
			<value><xsl:value-of select="./item[$I]" /></value>
		</availability>
	</xsl:template>
</xsl:stylesheet>