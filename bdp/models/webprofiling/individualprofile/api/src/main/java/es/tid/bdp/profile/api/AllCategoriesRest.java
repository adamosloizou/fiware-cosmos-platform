package es.tid.bdp.profile.api;

import java.util.List;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

/**
 *
 * @author dmicol
 */
@Path("/{username}/all/{n}")
@Component
@Scope("request")
public class AllCategoriesRest {
    @Autowired(required = true)
    private ProfileDAO profile;

    public void setDao(ProfileDAO dao) {
        this.profile = dao;
    }

    @GET
    @Produces(MediaType.APPLICATION_JSON)
    public List getAll(@PathParam("username") String userName) {
        return Categories.getAll(this.profile, userName);
    }
}
