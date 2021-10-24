CREATE TABLE series_name(
	SOPInstanceUID VARCHAR(64) NOT NULL UNIQUE,
	imagePositionPatient VARCHAR(64) NOT NULL,
	imageOrientationPatient VARCHAR(16) NOT NULL,
	localFilePath VARCHAR(256) NOT NULL,
	PRIMARY KEY(SOPInstanceUID)
);