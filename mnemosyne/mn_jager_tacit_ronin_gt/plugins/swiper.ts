const plugin = require('tailwindcss/plugin')

module.exports = plugin(function ({ addComponents, theme }) {
    addComponents({
        '.swiper-pagination-bullets': {
            '.swiper-pagination-bullet': {
                '@apply bg-custom-500 h-4 w-4 border-4 dark:border-zinc-900 outline outline-1 outline-custom-500': {},
            }
        },
        '.swiper-button-prev:after, .swiper-button-next:after': {
            '@apply !text-lg': {},
            
        }
    })
})