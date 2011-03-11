<?xml version="1.0" encoding="UTF-8"?>
<!-- Author: University of Salento and CMCC -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" omit-xml-declaration="no" indent="yes" />
	
	<xsl:template match="/result/item">
		<results>
			<xsl:apply-templates select="activityRecords" />
			<xsl:copy-of select="totalCount" />
		</results>
	</xsl:template>
	
	<xsl:template match="/result/item/activityRecords">
		<xsl:copy>
			<xsl:apply-templates select="item" />
		</xsl:copy>		
	</xsl:template>
	
	<xsl:template match="/result/item/activityRecords/item">
		<xsl:copy>
			<xsl:copy-of select="startDate" />
			<xsl:copy-of select="startTime" />
			<xsl:copy-of select="endDate" />
			<xsl:copy-of select="endTime" />
			<xsl:apply-templates select="values" />
		</xsl:copy>
	</xsl:template>
	
	<xsl:template match="/result/item/activityRecords/item/values">
		<serviceAvailable><xsl:value-of select="./item[1]" /></serviceAvailable>
		<serviceDown><xsl:value-of select="./item[2]" /></serviceDown>
		<hostUnreachable><xsl:value-of select="./item[3]" /></hostUnreachable>
		<networkErrors><xsl:value-of select="./item[4]" /></networkErrors>
	</xsl:template>	
</xsl:stylesheet>
