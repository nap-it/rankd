use warp::Filter;
use slog::{Logger, info};

pub async fn start(log: Logger) {
    let testing_tool = warp::path::end().map(move || {
        format!("Welcome to this service")
    });

    let kernel_info = warp::path("kernel").map(move || {
        format!("{:#?}", crate::capabilities::Capabilities::kernel())
    });

    let userspace_info = warp::path("user").map(move || {
        format!("{:#?}", crate::capabilities::Capabilities::user())
    });

    let routes = warp::get().and(
        kernel_info
            .or(userspace_info)
    );

    warp::serve(routes).run(([127, 0, 0, 1], 7265)).await;
}