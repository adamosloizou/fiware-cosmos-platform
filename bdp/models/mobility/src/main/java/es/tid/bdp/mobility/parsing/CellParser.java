package es.tid.bdp.mobility.parsing;

import es.tid.bdp.mobility.data.MobProtocol.Cell;

/**
 *
 * @author sortega
 */
public class CellParser extends PipeDelimitedParser {
    public CellParser(String line) {
        super(line);
    }

    @Override
    public Cell parse() {
        return Cell.newBuilder()
            .setCellId(parseCellId())
            .setPlaceId(parseLong())
            .setGeoloc1(parseInt())
            .setGeoloc2(parseInt())
            .setPosx(parseInt())
            .setPosy(parseInt())
            .build();
    }
}
