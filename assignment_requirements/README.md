# Text for Homework 1 at PCom

## How to use

We use Markdown for the homework text. We use pull requests to update the content. The `main` branch is automatically published at the following address: https://pcom.pages.upb.ro/tema1/
We recommend using [language tool](https://languagetool.org/) for spell checking.

### Directly (easies)

We can edit the contents directly from the browser by using the GitLab IDE. The content for each lab is in the `src` folder. For example,
to edit the first lab, we can go to any of the [lab's markdown files](https://gitlab.cs.pub.ro/pcom/labs/-/blob/main/src/lab1/about.md),
click on the `Open in Web IDE` button and edit the markdown. In a similar fashion, each page has an edit button on the top right corner,
you can press it to make changes to a page.

### Locally (hardest)

You have to install [mdbook](https://github.com/rust-lang/mdBook). The easiest way is to use cargo:

We install rust:
```
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

And then we install mdbook
```
cargo install mdbook
```

And finally in the source folder

```
mdbook serve
```
