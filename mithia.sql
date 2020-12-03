/*
 Navicat Premium Data Transfer

 Source Server         : vmlokal
 Source Server Type    : MySQL
 Source Server Version : 50732
 Source Host           : localhost:3306
 Source Schema         : mithia

 Target Server Type    : MySQL
 Target Server Version : 50732
 File Encoding         : 65001

 Date: 03/12/2020 20:02:20
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for AccountRegistry
-- ----------------------------
DROP TABLE IF EXISTS `AccountRegistry`;
CREATE TABLE `AccountRegistry`  (
  `ArgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ArgActId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ArgIdentifier` varchar(45) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ArgValue` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ArgPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`ArgId`) USING BTREE,
  INDEX `Account`(`ArgActId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for AdminPassword
-- ----------------------------
DROP TABLE IF EXISTS `AdminPassword`;
CREATE TABLE `AdminPassword`  (
  `AdmId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `AdmActId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `AdmPassword` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `AdmTimer` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`AdmId`) USING BTREE,
  INDEX `AdmActId`(`AdmActId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Aethers
-- ----------------------------
DROP TABLE IF EXISTS `Aethers`;
CREATE TABLE `Aethers`  (
  `AthId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `AthChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `AthSplId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `AthDuration` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `AthAether` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `AthPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`AthId`) USING BTREE,
  INDEX `AthSplId`(`AthSplId`) USING BTREE,
  INDEX `AthChaId`(`AthChaId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for AfkLogs
-- ----------------------------
DROP TABLE IF EXISTS `AfkLogs`;
CREATE TABLE `AfkLogs`  (
  `AfkId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `AfkDate` datetime(0) NULL DEFAULT NULL,
  `AfkChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `AfkMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `AfkX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `AfkY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`AfkId`) USING BTREE,
  INDEX `AfkMapId`(`AfkMapId`) USING BTREE,
  INDEX `AfkChaId`(`AfkChaId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of AfkLogs
-- ----------------------------
INSERT INTO `AfkLogs` VALUES (1, '2020-12-03 16:39:16', 2, 144, 6, 5);

-- ----------------------------
-- Table structure for ArmorClassification
-- ----------------------------
DROP TABLE IF EXISTS `ArmorClassification`;
CREATE TABLE `ArmorClassification`  (
  `ArcId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ArcName` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ArcLevel` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ArcLookColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`ArcId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for ArmorType
-- ----------------------------
DROP TABLE IF EXISTS `ArmorType`;
CREATE TABLE `ArmorType`  (
  `ArtId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ArtName` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ArtMark` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ArtLook` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ArtClass` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ArtSex` int(10) UNSIGNED NOT NULL DEFAULT 2,
  PRIMARY KEY (`ArtId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Authorize
-- ----------------------------
DROP TABLE IF EXISTS `Authorize`;
CREATE TABLE `Authorize`  (
  `AutId` int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  `AutChaId` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `AutChaName` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `AutIP` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `AutTimer` int(11) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`AutId`) USING BTREE,
  INDEX `AutChaName`(`AutChaName`) USING BTREE,
  INDEX `AutChaId`(`AutChaId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 30 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Banks
-- ----------------------------
DROP TABLE IF EXISTS `Banks`;
CREATE TABLE `Banks`  (
  `BnkId` int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  `BnkChaId` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `BnkItmId` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `BnkAmount` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `BnkChaIdOwner` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `BnkEngrave` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '0',
  `BnkTimer` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `BnkPosition` int(11) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`BnkId`) USING BTREE,
  INDEX `BnkChaId`(`BnkChaId`) USING BTREE,
  INDEX `BnkItmId`(`BnkItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for BoardNames
-- ----------------------------
DROP TABLE IF EXISTS `BoardNames`;
CREATE TABLE `BoardNames`  (
  `BnmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BnmIdentifier` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `BnmDescription` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `BnmLevel` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BnmGMLevel` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BnmPthId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BnmClnId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BnmScripted` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BnmSortOrder` int(10) UNSIGNED NOT NULL DEFAULT 256,
  PRIMARY KEY (`BnmId`) USING BTREE,
  INDEX `BnmClnId`(`BnmClnId`) USING BTREE,
  INDEX `BnmPthId`(`BnmPthId`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of BoardNames
-- ----------------------------
INSERT INTO `BoardNames` VALUES (0, 'nmail', 'Nexia Mail (NMail)', 5, 0, 0, 0, 0, 10);
INSERT INTO `BoardNames` VALUES (1, 'news_announcement', 'News & Announcements', 0, 0, 0, 0, 1, 20);
INSERT INTO `BoardNames` VALUES (2, 'scum_bad_player', 'Scum ', 0, 0, 0, 0, 1, 99);
INSERT INTO `BoardNames` VALUES (3, 'guide_board', 'Guides', 0, 0, 0, 0, 0, 40);
INSERT INTO `BoardNames` VALUES (4, 'lounge_board', 'Lounge', 10, 0, 0, 0, 0, 50);
INSERT INTO `BoardNames` VALUES (5, 'rules_law', 'Rules', 0, 0, 0, 0, 1, 100);
INSERT INTO `BoardNames` VALUES (6, 'discussion_board', 'General Discussion', 10, 0, 0, 0, 0, 80);
INSERT INTO `BoardNames` VALUES (7, 'report_board', 'Bug Reports', 85, 0, 0, 0, 0, 90);
INSERT INTO `BoardNames` VALUES (8, 'buy_board', 'Buy Market', 50, 0, 0, 0, 0, 91);
INSERT INTO `BoardNames` VALUES (9, 'sell_board', 'Sell Market', 50, 0, 0, 0, 0, 92);
INSERT INTO `BoardNames` VALUES (10, 'dream_board', 'Dreamweaver', 85, 0, 0, 0, 0, 93);
INSERT INTO `BoardNames` VALUES (11, 'event_board', 'Events', 10, 0, 0, 0, 1, 30);
INSERT INTO `BoardNames` VALUES (12, 'suggestion_board', 'Suggestions', 85, 0, 0, 0, 1, 94);

-- ----------------------------
-- Table structure for BoardTitles
-- ----------------------------
DROP TABLE IF EXISTS `BoardTitles`;
CREATE TABLE `BoardTitles`  (
  `BtlId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `BtlDescription` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`BtlId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Boards
-- ----------------------------
DROP TABLE IF EXISTS `Boards`;
CREATE TABLE `Boards`  (
  `BrdId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `BrdBnmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BrdChaName` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `BrdBtlId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BrdHighlighted` int(11) NOT NULL DEFAULT 0,
  `BrdMonth` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BrdDay` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BrdTopic` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `BrdPost` text CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL,
  `BrdPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`BrdId`) USING BTREE,
  INDEX `BrdBnmId`(`BrdBnmId`) USING BTREE,
  INDEX `BrdChaName`(`BrdChaName`) USING BTREE,
  INDEX `BrdBtlId`(`BrdBtlId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for BreakLogs
-- ----------------------------
DROP TABLE IF EXISTS `BreakLogs`;
CREATE TABLE `BreakLogs`  (
  `BrkId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `BrkDate` datetime(0) NULL DEFAULT NULL,
  `BrkChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BrkMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BrkX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BrkY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BrkItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`BrkId`) USING BTREE,
  INDEX `BrkChaId`(`BrkChaId`) USING BTREE,
  INDEX `BrkMapId`(`BrkMapId`) USING BTREE,
  INDEX `BrkItmId`(`BrkItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for BuyLogs
-- ----------------------------
DROP TABLE IF EXISTS `BuyLogs`;
CREATE TABLE `BuyLogs`  (
  `BuyId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `BuyDate` datetime(0) NULL DEFAULT NULL,
  `BuyChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BuyMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BuyX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BuyY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BuyItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BuyPrice` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `BuyAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`BuyId`) USING BTREE,
  INDEX `BuyChaId`(`BuyChaId`) USING BTREE,
  INDEX `BuyMapId`(`BuyMapId`) USING BTREE,
  INDEX `BuyItmId`(`BuyItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Character
-- ----------------------------
DROP TABLE IF EXISTS `Character`;
CREATE TABLE `Character`  (
  `ChaId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ChaName` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ChaPassword` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ChaBanned` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaClnId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaClanTitle` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ChaTitle` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ChaLevel` int(10) UNSIGNED NOT NULL DEFAULT 1,
  `ChaPthId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaMark` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaTier` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaTotem` int(10) UNSIGNED NOT NULL DEFAULT 4,
  `ChaKarma` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaCurrentVita` int(10) UNSIGNED NOT NULL DEFAULT 100,
  `ChaMaximumVita` int(10) UNSIGNED NOT NULL DEFAULT 100,
  `ChaCurrentMana` int(10) UNSIGNED NOT NULL DEFAULT 50,
  `ChaMaximumMana` int(10) UNSIGNED NOT NULL DEFAULT 50,
  `ChaExperience` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaGold` int(10) UNSIGNED NOT NULL DEFAULT 100,
  `ChaSex` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaNation` int(10) UNSIGNED NOT NULL DEFAULT 5,
  `ChaFace` int(10) UNSIGNED NOT NULL DEFAULT 200,
  `ChaFaceColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaHair` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaHairColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaSkinColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaArmorColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaSide` int(10) UNSIGNED NOT NULL DEFAULT 2,
  `ChaState` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaOnline` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaChaIdSpouse` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaClanChat` int(10) UNSIGNED NOT NULL DEFAULT 1,
  `ChaPathChat` int(10) UNSIGNED NOT NULL DEFAULT 1,
  `ChaActId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaSettings` int(10) UNSIGNED NOT NULL DEFAULT 12605,
  `ChaGMLevel` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaF1Name` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ChaDisguise` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaDisguiseColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaPK` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaKilledBy` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaKillsPK` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaPKDuration` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaMuted` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaHeroes` int(10) UNSIGNED NOT NULL DEFAULT 1,
  `ChaMaximumInventory` int(10) UNSIGNED NOT NULL DEFAULT 27,
  `ChaMaximumBankSlots` int(10) UNSIGNED NOT NULL DEFAULT 15,
  `ChaBankGold` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ChaExperienceSold` bigint(20) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`ChaId`) USING BTREE,
  INDEX `ChaActId`(`ChaActId`) USING BTREE,
  INDEX `ChaName`(`ChaName`) USING BTREE,
  INDEX `ChaClnId`(`ChaClnId`) USING BTREE,
  INDEX `ChaPthId`(`ChaPthId`) USING BTREE,
  INDEX `ChaMapId`(`ChaMapId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Character
-- ----------------------------
INSERT INTO `Character` VALUES (1, 'Admin', '1040ab931ebd81f6827805e9db63fb23', 0, 0, '', 'Peasant', 109, 0, 3, 0, 1, 0, 4063, 4181, 10209, 10209, 2000001, 866076, 0, 2, 200, 3, 16, 11, 64, 0, 144, 17, 33, 2, 0, 1, 0, 1, 0, 0, 4542, 99, 'Oldman', 2012, 2, 0, 1417, 0, 0, 0, 1, 51, 29, 800000000, 47888850000);

-- ----------------------------
-- Table structure for CharacterDeathDropLogs
-- ----------------------------
DROP TABLE IF EXISTS `CharacterDeathDropLogs`;
CREATE TABLE `CharacterDeathDropLogs`  (
  `CddId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `CddDate` datetime(0) NULL DEFAULT NULL,
  `CddChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `CddMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `CddX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `CddY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `CddItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `CddAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`CddId`) USING BTREE,
  INDEX `CddChaId`(`CddChaId`) USING BTREE,
  INDEX `CddMapId`(`CddMapId`) USING BTREE,
  INDEX `CddItmId`(`CddItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for ClanBanks
-- ----------------------------
DROP TABLE IF EXISTS `ClanBanks`;
CREATE TABLE `ClanBanks`  (
  `CbkId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `CbkClnId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `CbkItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `CbkAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `CbkPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`CbkId`) USING BTREE,
  INDEX `CbkClnId`(`CbkClnId`) USING BTREE,
  INDEX `CbkItmId`(`CbkItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for ClanLogs
-- ----------------------------
DROP TABLE IF EXISTS `ClanLogs`;
CREATE TABLE `ClanLogs`  (
  `ClgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ClgDate` datetime(0) NULL DEFAULT NULL,
  `ClgChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ClgClnId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ClgMessage` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`ClgId`) USING BTREE,
  INDEX `ClgChaId`(`ClgChaId`) USING BTREE,
  INDEX `ClgClnId`(`ClgClnId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Clans
-- ----------------------------
DROP TABLE IF EXISTS `Clans`;
CREATE TABLE `Clans`  (
  `ClnId` int(10) UNSIGNED NOT NULL,
  `ClnName` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ClnBankSlots` int(10) UNSIGNED NOT NULL DEFAULT 50,
  PRIMARY KEY (`ClnId`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for DropLogs
-- ----------------------------
DROP TABLE IF EXISTS `DropLogs`;
CREATE TABLE `DropLogs`  (
  `DrpId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `DrpDate` datetime(0) NULL DEFAULT NULL,
  `DrpChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `DrpMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `DrpX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `DrpY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `DrpItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `DrpAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`DrpId`) USING BTREE,
  INDEX `DrpChaId`(`DrpChaId`) USING BTREE,
  INDEX `DrpMapId`(`DrpMapId`) USING BTREE,
  INDEX `DrpItmId`(`DrpItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of DropLogs
-- ----------------------------
INSERT INTO `DropLogs` VALUES (1, '2020-12-03 19:14:19', 1, 144, 16, 37, 0, 1000);
INSERT INTO `DropLogs` VALUES (2, '2020-12-03 19:14:26', 1, 144, 17, 37, 0, 100000);
INSERT INTO `DropLogs` VALUES (3, '2020-12-03 19:14:34', 1, 144, 17, 37, 0, 866076);
INSERT INTO `DropLogs` VALUES (4, '2020-12-03 19:14:51', 1, 144, 17, 37, 0, 10000);

-- ----------------------------
-- Table structure for Enchantments
-- ----------------------------
DROP TABLE IF EXISTS `Enchantments`;
CREATE TABLE `Enchantments`  (
  `EncId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `EncName` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `EncMark` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncClass` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncVita` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncMana` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncMight` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncGrace` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncWill` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncWisdom` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncCon` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncProtection` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncArmor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncHit` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncDam` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncMinimumDamage` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncMaximumDamage` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EncApplyto` int(10) UNSIGNED NOT NULL DEFAULT 31 COMMENT '0 = applies to all equip\n1 = armor\n2 = weap\n4 = helm\n8 = gloves\n16 = subaccess',
  PRIMARY KEY (`EncId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for EquipLogs
-- ----------------------------
DROP TABLE IF EXISTS `EquipLogs`;
CREATE TABLE `EquipLogs`  (
  `ElgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ElgDate` datetime(0) NULL DEFAULT NULL,
  `ElgChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ElgMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ElgX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ElgY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ElgItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`ElgId`) USING BTREE,
  INDEX `ElgChaId`(`ElgChaId`) USING BTREE,
  INDEX `ElgMapId`(`ElgMapId`) USING BTREE,
  INDEX `ElgItmId`(`ElgItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Equipment
-- ----------------------------
DROP TABLE IF EXISTS `Equipment`;
CREATE TABLE `Equipment`  (
  `EqpId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `EqpChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EqpItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EqpDurability` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EqpChaIdOwner` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EqpEngrave` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `EqpTimer` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `EqpSlot` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`EqpId`) USING BTREE,
  INDEX `EqpChaId`(`EqpChaId`) USING BTREE,
  INDEX `EqpItmId`(`EqpItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for ExchangeLogs
-- ----------------------------
DROP TABLE IF EXISTS `ExchangeLogs`;
CREATE TABLE `ExchangeLogs`  (
  `ExgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ExgDate` datetime(0) NULL DEFAULT NULL,
  `ExgChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ExgMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ExgX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ExgY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ExgTarget` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ExgMapIdTarget` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ExgXTarget` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ExgYTarget` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ExgItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ExgAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`ExgId`) USING BTREE,
  INDEX `ExgChaId`(`ExgChaId`) USING BTREE,
  INDEX `ExgMapId`(`ExgMapId`) USING BTREE,
  INDEX `ExgItmId`(`ExgItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Friends
-- ----------------------------
DROP TABLE IF EXISTS `Friends`;
CREATE TABLE `Friends`  (
  `FndId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `FndChaId` int(10) UNSIGNED NOT NULL,
  `FndChaName1` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName2` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName3` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName4` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName5` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName6` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName7` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName8` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName9` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName10` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName11` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName12` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName13` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName14` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName15` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName16` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName17` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName18` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName19` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `FndChaName20` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`FndId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 3 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Friends
-- ----------------------------
INSERT INTO `Friends` VALUES (1, 2, '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '');
INSERT INTO `Friends` VALUES (2, 1, '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '');

-- ----------------------------
-- Table structure for GameRegistry
-- ----------------------------
DROP TABLE IF EXISTS `GameRegistry`;
CREATE TABLE `GameRegistry`  (
  `GrgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `GrgIdentifier` varchar(45) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `GrgValue` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`GrgId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for HandLogs
-- ----------------------------
DROP TABLE IF EXISTS `HandLogs`;
CREATE TABLE `HandLogs`  (
  `HndId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `HndDate` datetime(0) NULL DEFAULT NULL,
  `HndChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `HndMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `HndX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `HndY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `HndItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `HndAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `HndTarget` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `HndMapIdTarget` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `HndXTarget` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `HndYTarget` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`HndId`) USING BTREE,
  INDEX `HndChaId`(`HndChaId`) USING BTREE,
  INDEX `HndMapId`(`HndMapId`) USING BTREE,
  INDEX `HndItmId`(`HndItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Inventory
-- ----------------------------
DROP TABLE IF EXISTS `Inventory`;
CREATE TABLE `Inventory`  (
  `InvId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `InvChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `InvItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `InvAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `InvDurability` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `InvChaIdOwner` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `InvEngrave` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `InvTimer` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `InvPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`InvId`) USING BTREE,
  INDEX `InvChaId`(`InvChaId`) USING BTREE,
  INDEX `InvItmId`(`InvItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Inventory
-- ----------------------------
INSERT INTO `Inventory` VALUES (1, 1, 122, 1, 0, 0, '', 0, 0);

-- ----------------------------
-- Table structure for Items
-- ----------------------------
DROP TABLE IF EXISTS `Items`;
CREATE TABLE `Items`  (
  `ItmId` int(10) UNSIGNED NOT NULL,
  `ItmPthId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmIdentifier` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ItmDescription` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ItmType` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmBuyPrice` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmSellPrice` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmMaximumAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmSex` int(10) UNSIGNED NOT NULL DEFAULT 2,
  `ItmLevel` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmLook` int(10) NOT NULL DEFAULT 0,
  `ItmLookColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmIcon` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmIconColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmSound` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmDurability` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmMight` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmGrace` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmWill` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmWisdom` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmCon` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmProtection` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmArmor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmHit` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmDam` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmAttackSpeed` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmMinimumDamage` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmMaximumDamage` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmVita` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmMana` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmVitaRequired` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmManaRequired` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmText` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ItmBuyText` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ItmExchangeable` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmDepositable` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmDroppable` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmThrown` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmUniqueAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmIndestructible` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmEvent` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmTimer` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmMark` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ItmChaIdDiscoverer` int(10) UNSIGNED NULL DEFAULT NULL,
  PRIMARY KEY (`ItmId`) USING BTREE,
  INDEX `ItmPthId`(`ItmPthId`) USING BTREE,
  INDEX `ItmChaId`(`ItmChaIdDiscoverer`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Items
-- ----------------------------
INSERT INTO `Items` VALUES (0, 0, 'gold', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (1, 0, 'gold_gt1', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (2, 0, 'gold_gt100', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (3, 0, 'gold_gt500', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (4, 0, 'gold_gt1000', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 73, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (5, 0, 'gold_gt5000', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 72, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (6, 0, 'gold_gt10000', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (7, 0, 'gold_gt50000', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 73, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (8, 0, 'gold_gt100000', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 72, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (9, 0, 'gold_gt500000', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (10, 0, 'gold_gt1000000', 'Gold', 0, 0, 0, 1, 2, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (120, 0, 'hard_coconut', 'Hard Coconut', 18, 500, 250, 30, 2, 0, 0, 0, 2267, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (121, 0, 'green_coco', 'Green Coconut', 18, 50, 25, 30, 2, 0, 0, 0, 2267, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (122, 0, 'coconut', 'Coconut', 18, 50, 25, 30, 2, 0, 0, 0, 2267, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (123, 0, 'bugs_liver', 'Bug\'s Liver', 18, 500, 250, 1, 2, 0, 0, 0, 220, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (124, 0, 'dung', 'Dung', 18, 10, 5, 1, 2, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (130, 0, 'trap_spot', 'Trap', 20, 100, 0, 1, 2, 0, 0, 0, 2272, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
INSERT INTO `Items` VALUES (171, 0, 'weather_crystal', 'Weather Crystal', 18, 1000, 100, 100, 2, 0, 0, 0, 428, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 'Temporary change weather', 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);

-- ----------------------------
-- Table structure for KillLogs
-- ----------------------------
DROP TABLE IF EXISTS `KillLogs`;
CREATE TABLE `KillLogs`  (
  `KlgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `KlgChaId` int(10) UNSIGNED NOT NULL,
  `KlgMobId` int(10) UNSIGNED NOT NULL,
  `KlgAmount` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`KlgId`) USING BTREE,
  INDEX `KlgChaId`(`KlgChaId`) USING BTREE,
  INDEX `KlgMobId`(`KlgMobId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Kills
-- ----------------------------
DROP TABLE IF EXISTS `Kills`;
CREATE TABLE `Kills`  (
  `KilId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `KilChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `KilMobId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `KilAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `KilPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`KilId`) USING BTREE,
  INDEX `KilChaId`(`KilChaId`) USING BTREE,
  INDEX `KilMobId`(`KilMobId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Legends
-- ----------------------------
DROP TABLE IF EXISTS `Legends`;
CREATE TABLE `Legends`  (
  `LegId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `LegChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `LegIdentifier` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `LegDescription` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `LegIcon` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `LegColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `LegPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`LegId`) USING BTREE,
  INDEX `LegChaId`(`LegChaId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Legends
-- ----------------------------
INSERT INTO `Legends` VALUES (1, 2, 'born', 'Born. (Sento 1, Fall)', 0, 80, 0);
INSERT INTO `Legends` VALUES (2, 3, 'born', 'Born. (Sento 1, Fall)', 0, 80, 0);
INSERT INTO `Legends` VALUES (3, 1, 'born', 'Born. (Sento 1, Fall)', 0, 80, 0);

-- ----------------------------
-- Table structure for LoginLogs
-- ----------------------------
DROP TABLE IF EXISTS `LoginLogs`;
CREATE TABLE `LoginLogs`  (
  `LgnId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `LgnDate` datetime(0) NULL DEFAULT NULL,
  `LgnChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `LgnIP` varchar(15) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `LgnActId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`LgnId`) USING BTREE,
  INDEX `LgnActId`(`LgnChaId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Mail
-- ----------------------------
DROP TABLE IF EXISTS `Mail`;
CREATE TABLE `Mail`  (
  `MalId` int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  `MalChaName` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `MalChaNameDestination` varchar(16) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `MalSubject` varchar(52) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `MalBody` varchar(4000) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `MalPosition` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `MalMonth` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `MalDay` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `MalDeleted` int(11) UNSIGNED NOT NULL DEFAULT 0,
  `MalNew` int(11) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`MalId`) USING BTREE,
  INDEX `MalChaName`(`MalChaName`) USING BTREE,
  INDEX `MalChaNameDestination`(`MalChaNameDestination`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for MapRegistry
-- ----------------------------
DROP TABLE IF EXISTS `MapRegistry`;
CREATE TABLE `MapRegistry`  (
  `MrgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `MrgMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MrgIdentifier` varchar(45) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `MrgValue` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MrgPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`MrgId`) USING BTREE,
  INDEX `MrgMapId`(`MrgMapId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of MapRegistry
-- ----------------------------
INSERT INTO `MapRegistry` VALUES (2, 3, 'tut_butcher_item_121', 21, 0);
INSERT INTO `MapRegistry` VALUES (3, 3, 'tut_butcher_item_122', 4, 1);

-- ----------------------------
-- Table structure for Maps
-- ----------------------------
DROP TABLE IF EXISTS `Maps`;
CREATE TABLE `Maps`  (
  `MapId` int(10) UNSIGNED NOT NULL,
  `MapName` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `MapBGM` int(10) UNSIGNED NOT NULL,
  `MapPvP` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MapSpells` int(10) UNSIGNED NOT NULL DEFAULT 1,
  `MapLight` int(10) UNSIGNED NOT NULL,
  `MapWeather` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MapSweepTime` int(10) UNSIGNED NOT NULL DEFAULT 7200000,
  `MapChat` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MapGhosts` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MapRegion` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MapIndoor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MapWarpout` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MapBind` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MapFile` varchar(1024) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`MapId`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Maps
-- ----------------------------
INSERT INTO `Maps` VALUES (0, 'Bliss', 1, 0, 1, 0, 0, 7200000, 0, 0, 0, 0, 1, 0, 'bliss.map');
INSERT INTO `Maps` VALUES (1, 'Island', 3, 0, 1, 0, 0, 7200000, 0, 0, 1, 0, 1, 0, 'region1/r1_island.map');
INSERT INTO `Maps` VALUES (2, 'Island Inn', 3, 0, 1, 25, 0, 7200000, 0, 0, 1, 1, 0, 0, 'region1/r1_islandinn.map');
INSERT INTO `Maps` VALUES (3, 'Island Shop', 3, 0, 1, 25, 0, 7200000, 0, 0, 1, 1, 0, 0, 'region1/r1_shop.map');
INSERT INTO `Maps` VALUES (4, 'Doctor Hut', 3, 0, 1, 25, 0, 7200000, 0, 0, 1, 1, 0, 0, 'region1/r1_doctorshut.map');
INSERT INTO `Maps` VALUES (5, 'Island Cave 1', 4, 0, 1, 25, 0, 7200000, 0, 0, 1, 1, 0, 0, 'region1/r1_cave1.map');
INSERT INTO `Maps` VALUES (6, 'Island Cave 2-1', 4, 0, 1, 25, 0, 7200000, 0, 0, 1, 1, 0, 0, 'region1/r1_cave2-1.map');
INSERT INTO `Maps` VALUES (11, 'Island Cave 2-2', 4, 0, 1, 25, 0, 7200000, 0, 0, 1, 1, 0, 0, 'region1/r1_cave2-2.map');
INSERT INTO `Maps` VALUES (12, 'Island Cave 3', 4, 0, 1, 25, 0, 7200000, 0, 0, 1, 1, 0, 0, 'region1/r1_cave3.map');
INSERT INTO `Maps` VALUES (13, 'Birdie Island', 5, 0, 1, 0, 0, 7200000, 0, 0, 1, 0, 1, 0, 'region1/r1_birdisland.map');
INSERT INTO `Maps` VALUES (144, 'Tutorial', 1, 0, 1, 0, 0, 7200000, 0, 0, 0, 0, 1, 0, 'region0/tutorial.map');
INSERT INTO `Maps` VALUES (145, 'Tutorial Hut', 2, 0, 1, 25, 0, 7200000, 0, 0, 0, 1, 0, 0, 'region0/tutorial_house.map');
INSERT INTO `Maps` VALUES (153, 'Tutorial Cave', 2, 0, 1, 25, 0, 7200000, 0, 0, 0, 1, 0, 0, 'region0/tutorial_cave.map');
INSERT INTO `Maps` VALUES (10000, 'Object Test', 4, 0, 1, 25, 0, 7200000, 0, 0, 1, 1, 0, 0, 'objtest.map');
INSERT INTO `Maps` VALUES (10001, 'Tile Test', 4, 0, 1, 25, 0, 7200000, 0, 0, 1, 1, 0, 0, 'tiletest.map');
INSERT INTO `Maps` VALUES (10010, 'Meeting Room', 6, 0, 1, 25, 0, 7200000, 0, 0, 1, 1, 0, 0, 'meeting_room.map');

-- ----------------------------
-- Table structure for MobDeathDropLogs
-- ----------------------------
DROP TABLE IF EXISTS `MobDeathDropLogs`;
CREATE TABLE `MobDeathDropLogs`  (
  `MddId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `MddDate` datetime(0) NULL DEFAULT NULL,
  `MddMobId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MddMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MddX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MddY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MddItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MddAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`MddId`) USING BTREE,
  INDEX `MddMobId`(`MddMobId`) USING BTREE,
  INDEX `MddMapId`(`MddMapId`) USING BTREE,
  INDEX `MddItmId`(`MddItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for MobEquipment
-- ----------------------------
DROP TABLE IF EXISTS `MobEquipment`;
CREATE TABLE `MobEquipment`  (
  `MeqId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `MeqMobId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MeqLook` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MeqColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MeqSlot` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`MeqId`) USING BTREE,
  INDEX `MeqMobId`(`MeqMobId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Mobs
-- ----------------------------
DROP TABLE IF EXISTS `Mobs`;
CREATE TABLE `Mobs`  (
  `MobId` int(10) UNSIGNED NOT NULL,
  `MobIdentifier` varchar(45) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `MobDescription` varchar(45) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `MobBehavior` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobAI` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobIsChar` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobIsNpc` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobLook` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobLookColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobVita` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobMana` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobExperience` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobHit` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobDam` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobLevel` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobMark` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobMinimumDamage` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobMaximumDamage` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobMoveTime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobAttackTime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobSpawnTime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobMight` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobGrace` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobWill` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobWisdom` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobCon` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobProtection` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobArmor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobSound` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobItmId1` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobAmount1` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobRate1` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobItmId2` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobAmount2` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobRate2` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobItmId3` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobAmount3` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobRate3` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobItmId4` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobAmount4` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobRate4` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobItmId5` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobAmount5` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobRate5` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobItmId6` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobAmount6` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobRate6` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobItmId7` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobAmount7` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobRate7` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobItmId8` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobAmount8` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobRate8` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobReturnDistance` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobSex` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobFace` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobFaceColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobHair` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobHairColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobSkinColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `MobState` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`MobId`) USING BTREE,
  INDEX `MobItmId1`(`MobItmId1`) USING BTREE,
  INDEX `MobItmId2`(`MobItmId2`) USING BTREE,
  INDEX `MobItmId3`(`MobItmId3`) USING BTREE,
  INDEX `MobItmId4`(`MobItmId4`) USING BTREE,
  INDEX `MobItmId5`(`MobItmId5`) USING BTREE,
  INDEX `MobItmId6`(`MobItmId6`) USING BTREE,
  INDEX `MobItmId7`(`MobItmId7`) USING BTREE,
  INDEX `MobItmId8`(`MobItmId8`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Mobs
-- ----------------------------
INSERT INTO `Mobs` VALUES (101, 'tutorial_tree', 'Tutorial Tree', 3, 4, 0, 0, 761, 0, 21, 30, 8, 0, 0, 0, 0, 0, 0, 5000, 5000, 30, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (102, 'squirrel_coco', 'Squirrel Coco', 0, 4, 0, 0, 25, 9, 14, 0, 15, 5, 1, 1, 0, 1, 5, 1500, 1300, 30, 0, 0, 0, 0, 0, 0, 0, 9, 120, 1, 30000, 121, 1, 40000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (103, 'tutorial_ore', 'Tutorial Ore', 3, 4, 0, 0, 853, 4, 21, 30, 8, 0, 0, 0, 0, 0, 0, 5000, 5000, 30, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (104, 'who_is_this', 'Who is This?', 0, 0, 0, 0, 1448, 0, 7, 0, 8, 1, 1, 1, 0, 1, 100, 1500, 1300, 10, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (105, 'tutorial_boat', 'Tutorial Boat', 0, 4, 0, 0, 1230, 0, 7, 0, 9, 1, 1, 1, 0, 1, 100, 1500, 1300, 10, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (106, 'bug2', 'Crawlers Bug', 0, 4, 0, 0, 183, 5, 21, 0, 15, 5, 1, 1, 0, 1, 10, 1500, 1300, 30, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (107, 'bugrare', 'Crawlers Rare Bug', 1, 4, 0, 0, 183, 3, 21, 0, 15, 5, 1, 1, 0, 1, 10, 1500, 1300, 30, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (108, 'squirrel_leader', 'Squirrel Leader', 1, 0, 0, 0, 25, 30, 70, 100, 375, 5, 2, 1, 0, 5, 10, 1500, 1300, 60, 0, 0, 0, 0, 0, 0, 1000, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (109, 'green_worm', 'Green Worm', 1, 4, 0, 0, 442, 31, 70, 50, 30, 5, 1, 6, 0, 3, 15, 1500, 1300, 300, 0, 0, 0, 0, 0, 0, 200, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (110, 'tutorial_troll', 'Tutorial Troll', 1, 0, 0, 0, 525, 14, 35, 50, 150, 5, 2, 0, 0, 5, 10, 1500, 1300, 60, 0, 0, 0, 0, 0, 1, 1000, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (111, 'tutorial_sheep', 'Tutorial Sheep', 0, 4, 0, 0, 173, 8, 21, 30, 8, 0, 0, 0, 0, 0, 0, 5000, 5000, 10, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (112, 'practice_dummy', 'Practice Dummy', 0, 4, 0, 0, 1302, 0, 100000000, 100000000, 0, 1, 1, 0, 0, 0, 0, 2000, 2000, 30, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (113, 'red_worm', 'Red Worm', 1, 4, 0, 0, 442, 30, 105, 100, 30, 10, 2, 6, 0, 5, 30, 1500, 1300, 300, 0, 0, 0, 0, 0, 0, 200, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `Mobs` VALUES (114, 'tutorial_bird', 'Bird', 0, 4, 0, 0, 1010, 0, 70, 100, 0, 1, 1, 1, 0, 1, 100, 1500, 1300, 60, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

-- ----------------------------
-- Table structure for NPCDeathDropLogs
-- ----------------------------
DROP TABLE IF EXISTS `NPCDeathDropLogs`;
CREATE TABLE `NPCDeathDropLogs`  (
  `NddId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `NddDate` datetime(0) NULL DEFAULT NULL,
  `NddNpcId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NddMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NddX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NddY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NddItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NddAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`NddId`) USING BTREE,
  INDEX `NddNpcId`(`NddNpcId`) USING BTREE,
  INDEX `NddMapId`(`NddMapId`) USING BTREE,
  INDEX `NddItmId`(`NddItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for NPCEquipment
-- ----------------------------
DROP TABLE IF EXISTS `NPCEquipment`;
CREATE TABLE `NPCEquipment`  (
  `NeqId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `NeqNpcId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NeqLook` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NeqColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NeqSlot` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`NeqId`) USING BTREE,
  INDEX `NeqNpcId`(`NeqNpcId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for NPCRegistry
-- ----------------------------
DROP TABLE IF EXISTS `NPCRegistry`;
CREATE TABLE `NPCRegistry`  (
  `NrgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `NrgChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NrgIdentifier` varchar(45) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `NrgValue` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NrgPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`NrgId`) USING BTREE,
  INDEX `NrgChaId`(`NrgChaId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for NPCs
-- ----------------------------
DROP TABLE IF EXISTS `NPCs`;
CREATE TABLE `NPCs`  (
  `NpcId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `NpcIdentifier` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `NpcDescription` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `NpcType` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcTimer` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcLook` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcLookColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcSex` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcSide` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcState` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcFace` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcFaceColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcHair` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcHairColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcSkinColor` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `NpcIsChar` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`NpcId`) USING BTREE,
  INDEX `NpcMapId`(`NpcMapId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 17 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of NPCs
-- ----------------------------
INSERT INTO `NPCs` VALUES (1, 'f1npc', 'F1 NPC', 1, 0, 0, 0, 0, 838, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (2, 'f2npc', 'F2 NPC', 1, 0, 0, 0, 0, 654, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (3, 'tutorial_hut', 'Hut', 0, 145, 9, 7, 0, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (4, 'tutorial_wood', 'Wood', 0, 144, 43, 30, 60000, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (5, 'tutorial_raft', 'Raft', 0, 144, 66, 50, 60000, 4, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (6, 'huna', 'Huna', 0, 4, 3, 7, 0, 610, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (7, 'lolita', 'Lolita', 0, 3, 10, 6, 0, 607, 14, 1, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (8, 'tom', 'Tom', 0, 2, 14, 6, 60000, 556, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (9, 'tut_butcher', 'Sander', 0, 3, 6, 6, 60000, 609, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (10, 'tut_inn_keeper', 'Yong', 0, 2, 8, 6, 60000, 605, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (11, 'tut_quest_bird_worshipper', 'Yang', 0, 1, 35, 9, 0, 604, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (12, 'noling', 'Noling', 0, 1, 28, 66, 20000, 598, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (13, 'tut_quest_girl', 'Ailana', 0, 1, 49, 59, 30000, 596, 27, 1, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (14, 'tut_quest_lost_girl', 'Ailani', 0, 1, 59, 9, 0, 596, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (15, 'tut_smith_boy', 'Tod', 0, 3, 3, 7, 0, 597, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `NPCs` VALUES (16, 'weather_bird', 'Weather Bird', 0, 13, 11, 8, 0, 1010, 4, 0, 2, 0, 0, 0, 0, 0, 0, 0);

-- ----------------------------
-- Table structure for Paper
-- ----------------------------
DROP TABLE IF EXISTS `Paper`;
CREATE TABLE `Paper`  (
  `PapId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `PapInvId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `PapText` text CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL,
  `PapWidth` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `PapHeight` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`PapId`) USING BTREE,
  INDEX `PapInvId`(`PapInvId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Parcels
-- ----------------------------
DROP TABLE IF EXISTS `Parcels`;
CREATE TABLE `Parcels`  (
  `ParId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ParSender` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ParChaIdDestination` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ParItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ParAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ParChaIdOwner` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ParEngrave` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `ParNpc` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ParPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`ParId`) USING BTREE,
  INDEX `ParChaIdDestination`(`ParChaIdDestination`) USING BTREE,
  INDEX `ParItmId`(`ParItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for PathLogs
-- ----------------------------
DROP TABLE IF EXISTS `PathLogs`;
CREATE TABLE `PathLogs`  (
  `PlgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `PlgDate` datetime(0) NULL DEFAULT NULL,
  `PlgChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `PlgPthId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `PlgMessage` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`PlgId`) USING BTREE,
  INDEX `PlgChaId`(`PlgChaId`) USING BTREE,
  INDEX `PlgPthId`(`PlgPthId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Paths
-- ----------------------------
DROP TABLE IF EXISTS `Paths`;
CREATE TABLE `Paths`  (
  `PthId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `PthType` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `PthChat` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `PthIcon` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `PthMark0` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark1` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark2` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark3` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark4` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark5` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark6` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark7` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark8` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark9` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark10` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark11` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark12` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark13` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark14` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `PthMark15` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`PthId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Paths
-- ----------------------------
INSERT INTO `Paths` VALUES (0, 0, 1, 0, 'Peasant', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '');
INSERT INTO `Paths` VALUES (1, 1, 1, 0, 'Warrior', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '');
INSERT INTO `Paths` VALUES (2, 2, 1, 0, 'Rogue', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '');
INSERT INTO `Paths` VALUES (3, 3, 1, 0, 'Mage', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '');
INSERT INTO `Paths` VALUES (4, 4, 1, 0, 'Poet', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '');

-- ----------------------------
-- Table structure for PickupLogs
-- ----------------------------
DROP TABLE IF EXISTS `PickupLogs`;
CREATE TABLE `PickupLogs`  (
  `PicId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `PicDate` datetime(0) NULL DEFAULT NULL,
  `PicChaId` int(10) UNSIGNED NOT NULL,
  `PicMapId` int(10) UNSIGNED NOT NULL,
  `PicX` int(10) UNSIGNED NOT NULL,
  `PicY` int(10) UNSIGNED NOT NULL,
  `PicItmId` int(10) UNSIGNED NOT NULL,
  `PicAmount` int(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`PicId`) USING BTREE,
  INDEX `PicChaId`(`PicChaId`) USING BTREE,
  INDEX `PicMapId`(`PicMapId`) USING BTREE,
  INDEX `PicItmId`(`PicItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for QuestRegistry
-- ----------------------------
DROP TABLE IF EXISTS `QuestRegistry`;
CREATE TABLE `QuestRegistry`  (
  `QrgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `QrgChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `QrgIdentifier` varchar(45) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `QrgValue` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `QrgPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`QrgId`) USING BTREE,
  INDEX `QrgChaId`(`QrgChaId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of QuestRegistry
-- ----------------------------
INSERT INTO `QuestRegistry` VALUES (1, 1, 'tutorial', 1, 0);

-- ----------------------------
-- Table structure for ReceiveParcelLogs
-- ----------------------------
DROP TABLE IF EXISTS `ReceiveParcelLogs`;
CREATE TABLE `ReceiveParcelLogs`  (
  `RpcId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `RpcDate` datetime(0) NULL DEFAULT NULL,
  `RpcChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RpcMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RpcX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RpcY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RpcItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RpcAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RpcSender` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RpcNpc` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`RpcId`) USING BTREE,
  INDEX `RpcChaId`(`RpcChaId`) USING BTREE,
  INDEX `RpcMapId`(`RpcMapId`) USING BTREE,
  INDEX `RpcItmId`(`RpcItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Recipes
-- ----------------------------
DROP TABLE IF EXISTS `Recipes`;
CREATE TABLE `Recipes`  (
  `RecId` int(11) NOT NULL DEFAULT 0,
  `RecIdentifier` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `RecDescription` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `RecCritIdentifier` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `RecCritDescription` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `RecCraftTime` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RecSuccessRate` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RecSkillAdvance` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RecCritRate` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RecBonus` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RecSkillRequired` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RecTokensRequired` int(11) NOT NULL DEFAULT 0,
  `RecMaterial1` int(11) NOT NULL DEFAULT 0,
  `RecAmount1` int(11) NOT NULL DEFAULT 0,
  `RecMaterial2` int(11) NOT NULL DEFAULT 0,
  `RecAmount2` int(11) NOT NULL DEFAULT 0,
  `RecMaterial3` int(11) NOT NULL DEFAULT 0,
  `RecAmount3` int(11) NOT NULL DEFAULT 0,
  `RecMaterial4` int(11) NOT NULL DEFAULT 0,
  `RecAmount4` int(11) NOT NULL DEFAULT 0,
  `RecMaterial5` int(11) NOT NULL DEFAULT 0,
  `RecAmount5` int(11) NOT NULL DEFAULT 0,
  `RecSuperiorMaterial1` int(11) NOT NULL DEFAULT 0,
  `RecSuperiorAmount1` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`RecId`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Registry
-- ----------------------------
DROP TABLE IF EXISTS `Registry`;
CREATE TABLE `Registry`  (
  `RegId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `RegChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RegIdentifier` varchar(45) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `RegValue` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `RegPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`RegId`) USING BTREE,
  INDEX `RegChaId`(`RegChaId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 7 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Registry
-- ----------------------------
INSERT INTO `Registry` VALUES (2, 2, 'WPEtimes', 1, 0);
INSERT INTO `Registry` VALUES (5, 1, 'firstloginfail', 1, 0);
INSERT INTO `Registry` VALUES (6, 2, 'firstloginfail', 1, 1);

-- ----------------------------
-- Table structure for SayLogs
-- ----------------------------
DROP TABLE IF EXISTS `SayLogs`;
CREATE TABLE `SayLogs`  (
  `SayId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `SayDate` datetime(0) NULL DEFAULT NULL,
  `SayChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SayType` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `SayMessage` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`SayId`) USING BTREE,
  INDEX `SayChaId`(`SayChaId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 68 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of SayLogs
-- ----------------------------
INSERT INTO `SayLogs` VALUES (1, '2020-12-03 16:27:10', 2, 'Say', 'asdsa');
INSERT INTO `SayLogs` VALUES (2, '2020-12-03 16:27:13', 2, 'Say', 'aasd');
INSERT INTO `SayLogs` VALUES (3, '2020-12-03 17:17:20', 2, 'Say', 'asda');
INSERT INTO `SayLogs` VALUES (4, '2020-12-03 17:17:24', 2, 'Say', '/warp 0');
INSERT INTO `SayLogs` VALUES (5, '2020-12-03 17:18:43', 2, 'Slash', '/stealth');
INSERT INTO `SayLogs` VALUES (6, '2020-12-03 17:18:46', 2, 'Slash', '/stealth');
INSERT INTO `SayLogs` VALUES (7, '2020-12-03 17:18:51', 2, 'Slash', '/warp 0');
INSERT INTO `SayLogs` VALUES (8, '2020-12-03 17:30:04', 1, 'Say', '/onlinedt');
INSERT INTO `SayLogs` VALUES (9, '2020-12-03 17:30:07', 1, 'Slash', '/online');
INSERT INTO `SayLogs` VALUES (10, '2020-12-03 17:30:09', 1, 'Say', 'aaa');
INSERT INTO `SayLogs` VALUES (11, '2020-12-03 17:30:14', 1, 'Slash', '/warp 0');
INSERT INTO `SayLogs` VALUES (12, '2020-12-03 17:33:58', 1, 'Slash', '/warp 144');
INSERT INTO `SayLogs` VALUES (13, '2020-12-03 17:34:01', 1, 'Slash', '/speed 10');
INSERT INTO `SayLogs` VALUES (14, '2020-12-03 17:37:06', 1, 'Slash', '/warp 145');
INSERT INTO `SayLogs` VALUES (15, '2020-12-03 17:37:14', 1, 'Say', 'aa');
INSERT INTO `SayLogs` VALUES (16, '2020-12-03 17:37:43', 1, 'Slash', '/lua broadcast(-1, \"test:\")');
INSERT INTO `SayLogs` VALUES (17, '2020-12-03 17:37:56', 1, 'Slash', '/lua broadcast(-1, NPC(1).name)');
INSERT INTO `SayLogs` VALUES (18, '2020-12-03 17:38:00', 1, 'Slash', '/lua broadcast(-1, NPC(2).name)');
INSERT INTO `SayLogs` VALUES (19, '2020-12-03 17:38:02', 1, 'Slash', '/lua broadcast(-1, NPC(3).name)');
INSERT INTO `SayLogs` VALUES (20, '2020-12-03 17:38:05', 1, 'Slash', '/lua broadcast(-1, NPC(4).name)');
INSERT INTO `SayLogs` VALUES (21, '2020-12-03 17:38:30', 1, 'Slash', '/lua NPC(3).click(Player(1))');
INSERT INTO `SayLogs` VALUES (22, '2020-12-03 17:40:51', 1, 'Slash', '/reloadlua');
INSERT INTO `SayLogs` VALUES (23, '2020-12-03 17:40:51', 1, 'Say', 'a');
INSERT INTO `SayLogs` VALUES (24, '2020-12-03 17:41:10', 1, 'Slash', '/i wool 5');
INSERT INTO `SayLogs` VALUES (25, '2020-12-03 17:41:14', 1, 'Slash', '/i wool');
INSERT INTO `SayLogs` VALUES (26, '2020-12-03 17:41:15', 1, 'Say', 'aa');
INSERT INTO `SayLogs` VALUES (27, '2020-12-03 17:42:21', 1, 'Slash', '/lua NPC(-1).click(Player(1))');
INSERT INTO `SayLogs` VALUES (28, '2020-12-03 17:42:29', 1, 'Slash', '/lua NPC(2).click(Player(1))');
INSERT INTO `SayLogs` VALUES (29, '2020-12-03 17:42:35', 1, 'Slash', '/lua NPC(2\\3).click(Player(1))');
INSERT INTO `SayLogs` VALUES (30, '2020-12-03 17:42:41', 1, 'Slash', '/lua NPC(3).click(Player(1))');
INSERT INTO `SayLogs` VALUES (31, '2020-12-03 17:43:25', 1, 'Slash', '/lua f1npc.click(Player(1))');
INSERT INTO `SayLogs` VALUES (32, '2020-12-03 17:43:44', 1, 'Slash', '/lua f1npc.click(Player(1), NPC(1))');
INSERT INTO `SayLogs` VALUES (33, '2020-12-03 17:44:44', 1, 'Slash', '/reloadlua');
INSERT INTO `SayLogs` VALUES (34, '2020-12-03 17:44:44', 1, 'Say', 'aaa');
INSERT INTO `SayLogs` VALUES (35, '2020-12-03 17:44:45', 1, 'Slash', '/lua f1npc.click(Player(1), NPC(1))');
INSERT INTO `SayLogs` VALUES (36, '2020-12-03 17:45:07', 1, 'Say', '/onlinedt');
INSERT INTO `SayLogs` VALUES (37, '2020-12-03 17:45:09', 1, 'Slash', '/online');
INSERT INTO `SayLogs` VALUES (38, '2020-12-03 18:19:34', 1, 'Slash', '/i coconut');
INSERT INTO `SayLogs` VALUES (39, '2020-12-03 18:32:24', 2, 'Slash', '/warp 144');
INSERT INTO `SayLogs` VALUES (40, '2020-12-03 18:32:27', 2, 'Say', '/app admin');
INSERT INTO `SayLogs` VALUES (41, '2020-12-03 18:32:31', 2, 'Slash', '/speed 10');
INSERT INTO `SayLogs` VALUES (42, '2020-12-03 18:33:02', 2, 'Say', '/togglegfx');
INSERT INTO `SayLogs` VALUES (43, '2020-12-03 18:33:06', 2, 'Slash', '/gfxtoggle');
INSERT INTO `SayLogs` VALUES (44, '2020-12-03 18:33:16', 2, 'Say', 'ndis');
INSERT INTO `SayLogs` VALUES (45, '2020-12-03 18:33:21', 2, 'Slash', '/state 4');
INSERT INTO `SayLogs` VALUES (46, '2020-12-03 18:33:23', 2, 'Say', 'ndis');
INSERT INTO `SayLogs` VALUES (47, '2020-12-03 18:33:25', 2, 'Say', 'ndis');
INSERT INTO `SayLogs` VALUES (48, '2020-12-03 18:33:35', 1, 'Say', '/ndis');
INSERT INTO `SayLogs` VALUES (49, '2020-12-03 18:33:39', 1, 'Slash', '/state 4');
INSERT INTO `SayLogs` VALUES (50, '2020-12-03 18:39:50', 2, 'Slash', '/givespell invoke');
INSERT INTO `SayLogs` VALUES (51, '2020-12-03 18:39:55', 2, 'Say', '/givespells invoke');
INSERT INTO `SayLogs` VALUES (52, '2020-12-03 18:39:58', 2, 'Slash', '/givespell invoke');
INSERT INTO `SayLogs` VALUES (53, '2020-12-03 18:40:04', 2, 'Slash', '/givespell gateway');
INSERT INTO `SayLogs` VALUES (54, '2020-12-03 18:58:40', 1, 'Slash', '/state 0');
INSERT INTO `SayLogs` VALUES (55, '2020-12-03 19:12:57', 1, 'Slash', '/givespell soothe');
INSERT INTO `SayLogs` VALUES (56, '2020-12-03 19:13:09', 1, 'Slash', '/givespell gateway');
INSERT INTO `SayLogs` VALUES (57, '2020-12-03 19:41:55', 1, 'Slash', '/givespell soothe');
INSERT INTO `SayLogs` VALUES (58, '2020-12-03 19:42:12', 1, 'Slash', '/lua Player(1):addSpell(\"soothe\")');
INSERT INTO `SayLogs` VALUES (59, '2020-12-03 19:46:38', 1, 'Slash', '/lua givespell 1');
INSERT INTO `SayLogs` VALUES (60, '2020-12-03 19:46:47', 1, 'Slash', '/givespell 1');
INSERT INTO `SayLogs` VALUES (61, '2020-12-03 19:51:08', 1, 'Slash', '/givespell soothe');
INSERT INTO `SayLogs` VALUES (62, '2020-12-03 19:51:16', 1, 'Slash', '/givespell invoke');
INSERT INTO `SayLogs` VALUES (63, '2020-12-03 19:51:36', 1, 'Slash', '/givespell recovery');
INSERT INTO `SayLogs` VALUES (64, '2020-12-03 19:51:58', 1, 'Slash', '/deletespell recovery');
INSERT INTO `SayLogs` VALUES (65, '2020-12-03 19:56:35', 1, 'Slash', '/reloadlua');
INSERT INTO `SayLogs` VALUES (66, '2020-12-03 19:57:50', 1, 'Slash', '/givespell soothe');
INSERT INTO `SayLogs` VALUES (67, '2020-12-03 19:58:13', 1, 'Slash', '/speed 10');

-- ----------------------------
-- Table structure for SellLogs
-- ----------------------------
DROP TABLE IF EXISTS `SellLogs`;
CREATE TABLE `SellLogs`  (
  `SelId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `SelDate` datetime(0) NULL DEFAULT NULL,
  `SelChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SelMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SelX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SelY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SelItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SelPrice` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SelAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`SelId`) USING BTREE,
  INDEX `SelChaId`(`SelChaId`) USING BTREE,
  INDEX `SelMapId`(`SelMapId`) USING BTREE,
  INDEX `SelItmId`(`SelItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for SendParcelLogs
-- ----------------------------
DROP TABLE IF EXISTS `SendParcelLogs`;
CREATE TABLE `SendParcelLogs`  (
  `SpcId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `SpcDate` datetime(0) NULL DEFAULT NULL,
  `SpcSender` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpcMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpcX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpcY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpcItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpcAmount` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpcChaIdDestination` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpcNpc` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`SpcId`) USING BTREE,
  INDEX `SpcChaId`(`SpcChaIdDestination`) USING BTREE,
  INDEX `SpcMapId`(`SpcMapId`) USING BTREE,
  INDEX `SpcItmId`(`SpcItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Spawns
-- ----------------------------
DROP TABLE IF EXISTS `Spawns`;
CREATE TABLE `Spawns`  (
  `SpnId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `SpnMobId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpnMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpnX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpnY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpnLastDeath` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SpnStartTime` int(10) UNSIGNED NOT NULL DEFAULT 25,
  `SpnEndTime` int(10) UNSIGNED NOT NULL DEFAULT 25,
  `SpnMobIdReplace` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`SpnId`) USING BTREE,
  INDEX `SpnMobId`(`SpnMobId`) USING BTREE,
  INDEX `SpnMapId`(`SpnMapId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for SpellBook
-- ----------------------------
DROP TABLE IF EXISTS `SpellBook`;
CREATE TABLE `SpellBook`  (
  `SbkId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `SbkChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SbkSplId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SbkPosition` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`SbkId`) USING BTREE,
  INDEX `SbkChaId`(`SbkChaId`) USING BTREE,
  INDEX `SbkSplId`(`SbkSplId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 3 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of SpellBook
-- ----------------------------
INSERT INTO `SpellBook` VALUES (1, 1, 2, 1);
INSERT INTO `SpellBook` VALUES (2, 1, 11, 2);

-- ----------------------------
-- Table structure for SpellCasts
-- ----------------------------
DROP TABLE IF EXISTS `SpellCasts`;
CREATE TABLE `SpellCasts`  (
  `SctId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `SctChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SctSplId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SctValue` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`SctId`) USING BTREE,
  INDEX `SctChaId`(`SctChaId`) USING BTREE,
  INDEX `SctSplId`(`SctSplId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of SpellCasts
-- ----------------------------
INSERT INTO `SpellCasts` VALUES (1, 1, 1, 35);
INSERT INTO `SpellCasts` VALUES (2, 1, 2, 3);
INSERT INTO `SpellCasts` VALUES (3, 1, 11, 31);

-- ----------------------------
-- Table structure for SpellLogs
-- ----------------------------
DROP TABLE IF EXISTS `SpellLogs`;
CREATE TABLE `SpellLogs`  (
  `SlgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `SlgDate` datetime(0) NULL DEFAULT NULL,
  `SlgChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SlgSplId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SlgType` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '0',
  `SlgMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SlgX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SlgY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SlgText` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NULL DEFAULT NULL,
  PRIMARY KEY (`SlgId`) USING BTREE,
  INDEX `SlgChaId`(`SlgChaId`) USING BTREE,
  INDEX `SlgSplId`(`SlgSplId`) USING BTREE,
  INDEX `SlgMapId`(`SlgMapId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Spells
-- ----------------------------
DROP TABLE IF EXISTS `Spells`;
CREATE TABLE `Spells`  (
  `SplId` int(10) UNSIGNED NOT NULL,
  `SplIdentifier` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `SplDescription` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  `SplType` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SplPthId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SplQuestion` varchar(64) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT 'NO',
  `SplDispel` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SplAether` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SplMute` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `SplActive` int(10) UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (`SplId`) USING BTREE,
  INDEX `SplPthId`(`SplPthId`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Spells
-- ----------------------------
INSERT INTO `Spells` VALUES (1, 'soothe', 'Soothe', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (2, 'invoke', 'Invoke', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (3, 'brace', 'Brace', 2, 0, 'Target?', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (4, 'afflict', 'Afflict', 2, 0, 'Target?', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (5, 'bind', 'Bind', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (6, 'gateway', 'Gateway', 1, 0, 'Which Gate (N E S W)?', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (7, 'light', 'Light', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (8, 'sage', 'Sage', 1, 0, '>', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (9, 'comfort', 'Comfort', 2, 0, 'Target?', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (10, 'mount_fox', 'Mount Fox', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (11, 'recovery', 'Recovery', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (12, 'cub_trap', 'Cub Trap', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (13, 'defenders_stance', 'Defender Stance', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (14, 'file', 'File', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (15, 'guard', 'Guard', 2, 0, 'Target?', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (16, 'hilt_strike', 'Hilt Strike', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (17, 'infuse', 'Infuse', 2, 0, 'Target?', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (18, 'inhibit', 'Inhibit', 2, 0, 'Target?', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (19, 'leech_trap', 'Leech Trap', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (20, 'rampage', 'Rampage', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (21, 'sand_trap', 'Sand Trap', 5, 0, 'NO', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (22, 'singe', 'Singe', 2, 0, 'Target?', 0, 0, 0, 1);
INSERT INTO `Spells` VALUES (23, 'warriors_stance', 'Warriors Stance', 5, 0, 'NO', 0, 0, 0, 1);

-- ----------------------------
-- Table structure for ThrowLogs
-- ----------------------------
DROP TABLE IF EXISTS `ThrowLogs`;
CREATE TABLE `ThrowLogs`  (
  `ThwId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ThwDate` datetime(0) NULL DEFAULT NULL,
  `ThwChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ThwMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ThwX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ThwY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ThwItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ThwMapIdDestination` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ThwXDestination` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `ThwYDestination` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`ThwId`) USING BTREE,
  INDEX `ThwChaId`(`ThwChaId`) USING BTREE,
  INDEX `ThwMapId`(`ThwMapId`) USING BTREE,
  INDEX `ThwItmId`(`ThwItmId`) USING BTREE,
  INDEX `ThwMapIdDestination`(`ThwMapIdDestination`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for Time
-- ----------------------------
DROP TABLE IF EXISTS `Time`;
CREATE TABLE `Time`  (
  `TimId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `TimHour` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `TimDay` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `TimSeason` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `TimYear` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`TimId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for UnAfkLogs
-- ----------------------------
DROP TABLE IF EXISTS `UnAfkLogs`;
CREATE TABLE `UnAfkLogs`  (
  `UfkId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `UfkDate` datetime(0) NULL DEFAULT NULL,
  `UfkChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UfkMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UfkX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UfkY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`UfkId`) USING BTREE,
  INDEX `UfkChaId`(`UfkChaId`) USING BTREE,
  INDEX `UfkMapId`(`UfkMapId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of UnAfkLogs
-- ----------------------------
INSERT INTO `UnAfkLogs` VALUES (1, '2020-12-03 16:39:33', 2, 144, 6, 5);

-- ----------------------------
-- Table structure for UnequipLogs
-- ----------------------------
DROP TABLE IF EXISTS `UnequipLogs`;
CREATE TABLE `UnequipLogs`  (
  `UlgId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `UlgDate` datetime(0) NULL DEFAULT NULL,
  `UlgChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UlgMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UlgX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UlgY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UlgItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`UlgId`) USING BTREE,
  INDEX `UlgChaId`(`UlgChaId`) USING BTREE,
  INDEX `UlgMapId`(`UlgMapId`) USING BTREE,
  INDEX `UlgItmId`(`UlgItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for UpTime
-- ----------------------------
DROP TABLE IF EXISTS `UpTime`;
CREATE TABLE `UpTime`  (
  `UtmId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `UtmValue` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`UtmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of UpTime
-- ----------------------------
INSERT INTO `UpTime` VALUES (3, 1607000232);

-- ----------------------------
-- Table structure for UseLogs
-- ----------------------------
DROP TABLE IF EXISTS `UseLogs`;
CREATE TABLE `UseLogs`  (
  `UseId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `UseDate` datetime(0) NULL DEFAULT NULL,
  `UseChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UseMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UseX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UseY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `UseItmId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`UseId`) USING BTREE,
  INDEX `UseChaId`(`UseChaId`) USING BTREE,
  INDEX `UseMapId`(`UseMapId`) USING BTREE,
  INDEX `UseItmId`(`UseItmId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for WarpLogs
-- ----------------------------
DROP TABLE IF EXISTS `WarpLogs`;
CREATE TABLE `WarpLogs`  (
  `WrpId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `WrpDate` datetime(0) NULL DEFAULT NULL,
  `WrpChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `WrpMapId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `WrpX` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `WrpY` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `WrpMapIdDestination` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `WrpXDestination` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `WrpYDestination` int(10) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`WrpId`) USING BTREE,
  INDEX `WrpChaId`(`WrpChaId`) USING BTREE,
  INDEX `WrpMapId`(`WrpMapId`) USING BTREE,
  INDEX `WrpMapIdDestination`(`WrpMapIdDestination`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for WhisperLogs
-- ----------------------------
DROP TABLE IF EXISTS `WhisperLogs`;
CREATE TABLE `WhisperLogs`  (
  `WspId` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `WspDate` datetime(0) NULL DEFAULT NULL,
  `WspChaId` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `WspChaIdDestination` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `WspMessage` varchar(255) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`WspId`) USING BTREE,
  INDEX `WspChaId`(`WspChaId`) USING BTREE,
  INDEX `WspChaIdDestination`(`WspChaIdDestination`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Triggers structure for table AfkLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `DateTimeNow`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `DateTimeNow` BEFORE INSERT ON `AfkLogs` FOR EACH ROW SET NEW.AfkDate = IFNULL(NEW.AfkDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table BreakLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `BrkDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `BrkDate` BEFORE INSERT ON `BreakLogs` FOR EACH ROW SET NEW.BrkDate = IFNULL(NEW.BrkDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table BuyLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `BuyDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `BuyDate` BEFORE INSERT ON `BuyLogs` FOR EACH ROW SET NEW.BuyDate = IFNULL(NEW.BuyDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table CharacterDeathDropLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `CddDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `CddDate` BEFORE INSERT ON `CharacterDeathDropLogs` FOR EACH ROW SET NEW.CddDate = IFNULL(NEW.CddDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table ClanLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `ClgDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `ClgDate` BEFORE INSERT ON `ClanLogs` FOR EACH ROW SET NEW.ClgDate = IFNULL(NEW.ClgDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table DropLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `DrpDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `DrpDate` BEFORE INSERT ON `DropLogs` FOR EACH ROW SET NEW.DrpDate = IFNULL(NEW.DrpDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table EquipLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `ElgDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `ElgDate` BEFORE INSERT ON `EquipLogs` FOR EACH ROW SET NEW.ElgDate = IFNULL(NEW.ElgDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table ExchangeLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `ExgDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `ExgDate` BEFORE INSERT ON `ExchangeLogs` FOR EACH ROW SET NEW.ExgDate = IFNULL(NEW.ExgDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table HandLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `HndDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `HndDate` BEFORE INSERT ON `HandLogs` FOR EACH ROW SET NEW.HndDate = IFNULL(NEW.HndDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table LoginLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `LgnDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `LgnDate` BEFORE INSERT ON `LoginLogs` FOR EACH ROW SET NEW.LgnDate = IFNULL(NEW.LgnDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table MobDeathDropLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `MddDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `MddDate` BEFORE INSERT ON `MobDeathDropLogs` FOR EACH ROW SET NEW.MddDate = IFNULL(NEW.MddDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table NPCDeathDropLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `NddDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `NddDate` BEFORE INSERT ON `NPCDeathDropLogs` FOR EACH ROW SET NEW.NddDate = IFNULL(NEW.NddDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table PathLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `PlgDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `PlgDate` BEFORE INSERT ON `PathLogs` FOR EACH ROW SET NEW.PlgDate = IFNULL(NEW.PlgDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table PickupLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `PicDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `PicDate` BEFORE INSERT ON `PickupLogs` FOR EACH ROW SET NEW.PicDate = IFNULL(NEW.PicDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table ReceiveParcelLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `RpcDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `RpcDate` BEFORE INSERT ON `ReceiveParcelLogs` FOR EACH ROW SET NEW.RpcDate = IFNULL(NEW.RpcDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table SayLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `SayDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `SayDate` BEFORE INSERT ON `SayLogs` FOR EACH ROW SET NEW.SayDate = IFNULL(NEW.SayDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table SellLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `SelDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `SelDate` BEFORE INSERT ON `SellLogs` FOR EACH ROW SET NEW.SelDate = IFNULL(NEW.SelDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table SendParcelLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `SpcDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `SpcDate` BEFORE INSERT ON `SendParcelLogs` FOR EACH ROW SET NEW.SpcDate = IFNULL(NEW.SpcDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table SpellLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `SlgDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `SlgDate` BEFORE INSERT ON `SpellLogs` FOR EACH ROW SET NEW.SlgDate = IFNULL(NEW.SlgDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table ThrowLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `ThwDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `ThwDate` BEFORE INSERT ON `ThrowLogs` FOR EACH ROW SET NEW.ThwDate = IFNULL(NEW.ThwDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table UnAfkLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `UfkDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `UfkDate` BEFORE INSERT ON `UnAfkLogs` FOR EACH ROW SET NEW.UfkDate = IFNULL(NEW.UfkDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table UnequipLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `UlgDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `UlgDate` BEFORE INSERT ON `UnequipLogs` FOR EACH ROW SET NEW.UlgDate = IFNULL(NEW.UlgDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table UseLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `UseDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `UseDate` BEFORE INSERT ON `UseLogs` FOR EACH ROW SET NEW.UseDate = IFNULL(NEW.UseDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table WarpLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `WrpDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `WrpDate` BEFORE INSERT ON `WarpLogs` FOR EACH ROW SET NEW.WrpDate = IFNULL(NEW.WrpDate, NOW())
;
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table WhisperLogs
-- ----------------------------
DROP TRIGGER IF EXISTS `WspDate`;
delimiter ;;
CREATE DEFINER = `root`@`localhost` TRIGGER `WspDate` BEFORE INSERT ON `WhisperLogs` FOR EACH ROW SET NEW.WspDate = IFNULL(NEW.WspDate, NOW())
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
