<!--
 * @Author: pyb0924
 * @Date: 2021-10-06 13:20:27
 * @LastEditTime: 2021-10-06 13:21:32
 * @Description: desc
 * @FilePath: \BM425\README.md
-->
# BM425 生物医学工程课程设计（影像方向）

## [DIBTest](./DIBTest)
位图的格式解析和使用

## [miniPACS](./miniPACS)
使用[Orthanc](https://www.orthanc-server.com/)和[DCMTK](https://www.dcmtk.org/)搭建的一个mini-PACS系统

## [Dicom_Transfer](./Dicom_Transfer/Dicom_Transfer)

一个基于DCMTK，[boost](https://www.boost.org/)和[MySQL](https://www.mysql.com/)的DICOM文件传输、压缩和存储实现。包括以下几个project：

- [DicomSCU](./Dicom_Transfer/Dicom_Transfer/DicomSCU)：基于DCMTK的dcmnet模块实现，实现了一个STORE SCU。
- [DicomSCP](./Dicom_Transfer/Dicom_Transfer/DicomSCP)：基于DCMTK的storescp实现，使其支持写入数据库和使用JPEG压缩。但由于storescp的命令行过于复杂，实现通过命令行参数控制是否使用JPEG压缩和写入DB比较困难，因此考虑直接基于dcmnet模块实现，即MySCP工程。
- [MySCP](.//Dicom_Transfer/Dicom_Transfer/MySCP)：基于DCMTK的dcmnet模块实现，实现了一个STORE SCP，其中集成了是否使用JPEG压缩和是否存入DB的选项。
- [DicomDB](./Dicom_Transfer/Dicom_Transfer/DicomDB)：将DICOM图像数据的Patient、Study、Image索引信息进入数据库管理。其中数据库部分使用本地的MySQL服务，Connector采用了[MySQL X DevAPI](https://dev.mysql.com/doc/x-devapi-userguide/en/)。
- [DicomJPEG](./Dicom_Transfer/Dicom_Transfer/DicomJPEG)：对DICOM图像的像素矩阵进行压缩，使用DCMTK自带的dcmjpeg模块实现。
- [DicomTransform](./Dicom_Transfer/Dicom_Transfer/DicomTransform)：一个对DICOM图像像素矩阵的几何变换实现，实现了图像的翻转（水平、竖直）和90度旋转（顺时针、逆时针）。
