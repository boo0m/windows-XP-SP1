if exists (select * from sysobjects where id = object_id(N'[dbo].[TblRCIncidentsQryFieldSE]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[TblRCIncidentsQryFieldSE]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[TblRCIncidents]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[TblRCIncidents]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[TblQryOP]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[TblQryOP]
GO

CREATE TABLE [dbo].[TblRCIncidentsQryFieldSE] (
	[iFieldID] [int] IDENTITY (1, 1) NOT NULL ,
	[sFieldName] [nvarchar] (50) NOT NULL ,
	[iOpType] [tinyint] NOT NULL ,
	[sTableName] [nvarchar] (100) NOT NULL ,
	[sFriendlyName] [nvarchar] (100) NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[TblRCIncidents] (
	[iIncidentID] [int] IDENTITY (1, 1) NOT NULL ,
	[sUserName] [nvarchar] (200) NOT NULL ,
	[sDescription] [nvarchar] (500) NULL ,
	[dtUploadDate] [smalldatetime] NOT NULL ,
	[sFile] [nvarchar] (500) NULL ,
	[iMemberHigh] [int] NULL ,
	[iMemberLow] [int] NULL ,
	[iStatus] [tinyint] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[TblQryOP] (
	[iOpID] [int] IDENTITY (1, 1) NOT NULL ,
	[sOpName] [nvarchar] (20) NULL ,
	[iOpType] [tinyint] NULL 
)
GO

ALTER TABLE [dbo].[TblRCIncidents] WITH NOCHECK ADD 
	CONSTRAINT [DF_Incidents_dtUploadDate] DEFAULT (getdate()) FOR [dtUploadDate],
	CONSTRAINT [DF_Incidents_iStatus] DEFAULT (0) FOR [iStatus]
GO

ALTER TABLE [dbo].[TblQryOP] WITH NOCHECK ADD 
	 PRIMARY KEY  CLUSTERED 
	(
		[iOpID]
	)  ON [PRIMARY] 
GO

