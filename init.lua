-- Set up caching and leader key
vim.g.base46_cache = vim.fn.stdpath("data") .. "/base46/"
vim.g.mapleader = " "

-- Bootstrap lazy.nvim
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not vim.uv.fs_stat(lazypath) then
  local repo = "https://github.com/folke/lazy.nvim.git"
  vim.fn.system({ "git", "clone", "--filter=blob:none", repo, "--branch=stable", lazypath })
end
vim.opt.rtp:prepend(lazypath)

-- Lazy.nvim setup
require("lazy").setup({
  {
    "Diogo-ss/42-header.nvim",
    cmd = { "Stdheader" },
    keys = { "<F1>" },
    opts = {
      default_map = true,  -- Default mapping for <F1>
      auto_update = true,  -- Update header when saving
      user = "zatais",     -- Your username
      mail = "zatais@student.1337.ma",  -- Your email
    },
    config = function(_, opts)
      require("42header").setup(opts)
    end,
  },
  -- C Formatter (42 Norm)
  {
    "cacharle/c_formatter_42.vim",
    ft = { "c", "h" },  -- Load only for C files
    event = "VeryLazy",
  },

  -- NERDTree (File Explorer)
  {
    "preservim/nerdtree",
    cmd = "NERDTreeToggle",
  },

  -- NvChad plugins
  { "NvChad/NvChad", lazy = false, branch = "v2.5", import = "nvchad.plugins" },

  -- Treesitter for syntax highlighting
  { "nvim-treesitter/nvim-treesitter", build = ":TSUpdate" },
})

-- Configure Treesitter
require("nvim-treesitter.configs").setup({
  ensure_installed = { "c", "cpp" }, -- Add more languages as needed
  highlight = {
    enable = true, -- Enables syntax highlighting
    additional_vim_regex_highlighting = false,
  },
})

-- Configure Completion
local cmp = require("cmp")
cmp.setup({
  snippet = {
    expand = function(args)
      require("luasnip").lsp_expand(args.body)
    end,
  },
  mapping = cmp.mapping.preset.insert({
    ["<C-b>"] = cmp.mapping.scroll_docs(-4),
    ["<C-f>"] = cmp.mapping.scroll_docs(4),
    ["<C-Space>"] = cmp.mapping.complete(),
    ["<C-e>"] = cmp.mapping.abort(),
    ["<CR>"] = cmp.mapping.confirm({ select = true }),
  }),
  sources = cmp.config.sources({
    { name = "nvim_lsp" },
    { name = "luasnip" },
  }, {
    { name = "buffer" },
    { name = "path" },
  }),
})

-- Set the number of spaces per tab
vim.opt.tabstop = 4
vim.opt.shiftwidth = 4
vim.opt.expandtab = false
vim.opt.softtabstop = 4

-- Auto-Save on InsertLeave and TextChanged

-- Background Transparency Fix
vim.api.nvim_set_hl(0, "Normal", { bg = "none" })
vim.api.nvim_set_hl(0, "NormalFloat", { bg = "none" })

-- Ensure base46 config doesn't crash
pcall(function()
  dofile(vim.g.base46_cache .. "defaults")
  dofile(vim.g.base46_cache .. "statusline")
end)

-- Load additional configurations
pcall(require, "options")
pcall(require, "nvchad.autocmds")
vim.schedule(function()
  pcall(require, "mappings")
end)

-- Set transparency for terminal compatibility
vim.cmd [[
  hi Normal guibg=NONE ctermbg=NONE
  hi NormalNC guibg=NONE ctermbg=NONE
  hi EndOfBuffer guibg=NONE ctermbg=NONE
]]

-- Configure lspconfig for C only
require('lspconfig').clangd.setup {
  filetypes = { "c" },  -- Only enable clangd for C files
}

-- Autocommand to save the buffer on CursorHold and CursorHoldI
vim.g.gtags_executable = '/usr/bin/ctags'
vim.api.nvim_create_augroup('GtagsAutoCmds', { clear = true })
vim.api.nvim_create_autocmd({"CursorHold", "CursorHoldI"}, {
  group = 'GtagsAutoCmds',
  pattern = '*',
  callback = function()
    if vim.bo.modified then  -- Check if the buffer is modified
      vim.cmd('silent! wa')  -- Save the buffer silently
    end
  end,
})

-- C_formatter_42

vim.g.c_formatter_42_set_equalprg = 1
