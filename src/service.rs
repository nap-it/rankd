use warp::Filter;
use slog::{Logger, info};

pub async fn start(log: Logger) {
    //let testing_tool = warp::path::end().map(move || {
    //    info!(log, "Received a GET request on /.");
    //    format!("Welcome to this service")
    //});

    let kernel_info = warp::path("kernel").map(move || {
        info!(log, "Received a GET request on /kernel.");
        format!("{:#?}", crate::capabilities::Capabilities::kernel())
    });

    let routes = warp::get().and(
        kernel_info
    );

    warp::serve(routes).run(([127, 0, 0, 1], 7265)).await;
}