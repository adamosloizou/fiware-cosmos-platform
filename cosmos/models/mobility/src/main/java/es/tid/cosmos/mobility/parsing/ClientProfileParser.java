/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;

/**
 *
 * @author dmicol
 */
class ClientProfileParser extends Parser {
    public ClientProfileParser(String line, String separator) {
        super(line, separator);
    }

    @Override
    public ClientProfile parse() {
        try {
            long userId = this.parseLong();
            int profile = this.parseInt();
            return ClientProfile.newBuilder()
                    .setProfileId(profile)
                    .setUserId(userId)
                    .build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line,
                                               ex);
        }
    }
}
