/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025 Chris Rizzitello <sithlord48@gmail.com>
 * SPDX-FileCopyrightText: (C) 2014 - 2016 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "ServerTests.h"

#include "common/KeyModifierID.h"
#include "deskflow/OptionTypes.h"
#include "server/Server.h"

void ServerTests::SwitchToComputerInfo_alloc_computer()
{
  auto actual = new Server::SwitchToComputerInfo("test");
  QCOMPARE(actual->m_computer, "test");
  delete actual;
}

void ServerTests::KeyboardBroadcastInfo_alloc_stateAndComputers()
{
  auto info = new Server::KeyboardBroadcastInfo(Server::KeyboardBroadcastInfo::State::kOn, "test");
  QCOMPARE(info->m_state, Server::KeyboardBroadcastInfo::State::kOn);
  QCOMPARE(info->m_computers, "test");
  delete info;
}

void ServerTests::mapHomeEndToLineNavigation_homeAndEnd()
{
  KeyID id = kKeyHome;
  KeyModifierMask mask = 0;
  QVERIFY(Server::mapHomeEndToLineNavigation(id, mask));
  QCOMPARE(id, kKeyLeft);
  QCOMPARE(mask, KeyModifierSuper);

  id = kKeyEnd;
  mask = 0;
  QVERIFY(Server::mapHomeEndToLineNavigation(id, mask));
  QCOMPARE(id, kKeyRight);
  QCOMPARE(mask, KeyModifierSuper);
}

void ServerTests::mapHomeEndToLineNavigation_keepsShift()
{
  KeyID id = kKeyHome;
  KeyModifierMask mask = KeyModifierShift;
  QVERIFY(Server::mapHomeEndToLineNavigation(id, mask));
  QCOMPARE(id, kKeyLeft);
  QCOMPARE(mask, KeyModifierShift | KeyModifierSuper);
}

void ServerTests::mapHomeEndToLineNavigation_ignoresShortcutsAndOtherKeys()
{
  KeyID id = kKeyHome;
  KeyModifierMask mask = KeyModifierControl;
  QVERIFY(!Server::mapHomeEndToLineNavigation(id, mask));
  QCOMPARE(id, kKeyHome);
  QCOMPARE(mask, KeyModifierControl);

  id = kKeyPageUp;
  mask = 0;
  QVERIFY(!Server::mapHomeEndToLineNavigation(id, mask));
  QCOMPARE(id, kKeyPageUp);
  QCOMPARE(mask, 0u);
}

void ServerTests::mapHomeEndToLineNavigation_customCommandMask()
{
  KeyID id = kKeyEnd;
  KeyModifierMask mask = KeyModifierShift;
  QVERIFY(Server::mapHomeEndToLineNavigation(id, mask, KeyModifierControl));
  QCOMPARE(id, kKeyRight);
  QCOMPARE(mask, KeyModifierShift | KeyModifierControl);
}

void ServerTests::serverMaskForClientSuper_default()
{
  deskflow::server::Config::ScreenOptions options;
  QCOMPARE(Server::serverMaskForClientSuper(options), KeyModifierSuper);

  options[kOptionModifierMapForControl] = kKeyModifierIDControl;
  options[kOptionModifierMapForSuper] = kKeyModifierIDSuper;
  QCOMPARE(Server::serverMaskForClientSuper(options), KeyModifierSuper);
}

void ServerTests::serverMaskForClientSuper_swappedCtrlAndSuper()
{
  deskflow::server::Config::ScreenOptions options;
  options[kOptionModifierMapForControl] = kKeyModifierIDSuper;
  options[kOptionModifierMapForSuper] = kKeyModifierIDControl;
  QCOMPARE(Server::serverMaskForClientSuper(options), KeyModifierControl);
}

void ServerTests::serverMaskForClientSuper_noModifierBecomesSuper()
{
  deskflow::server::Config::ScreenOptions options;
  options[kOptionModifierMapForSuper] = kKeyModifierIDControl;
  QCOMPARE(Server::serverMaskForClientSuper(options), 0u);
}

QTEST_MAIN(ServerTests)
